#include "Unit.hh"
#include "frontend/DiagnosticPrinter/DiagnosticPrinter.hh"
#include "frontend/Driver.hh"
#include <fmt/core.h>
#include <fstream>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/TargetParser/Host.h>
#include <mutex>
#include <utility>

using namespace llvm;

blang::Unit::Unit(std::string code, std::filesystem::path source_path,
                  bool debug, bool print_ast,
                  llvm::OptimizationLevel optimizationLevel,
                  std::filesystem::path build_directory)
    : context(new CompilerContext(source_path.filename().stem().string())),
      code(std::move(code)), debug(debug), print_ast(print_ast),
      source_filename(source_path.filename().string()),
      build_directory(std::move(build_directory)),
      optimizationLevel(optimizationLevel) {
  context->push_scope();

#if LLVM_VERSION_MAJOR > 20
  auto targetTriple = Triple(sys::getDefaultTargetTriple());
  context->fmodule.setTargetTriple(targetTriple);

  std::string err;
  target = TargetRegistry::lookupTarget(targetTriple, err);
  if (!target) {
    throw std::runtime_error(fmt::format("failed to lookup target {}: {}",
                                         targetTriple.getTriple(), err));
  }

  TargetOptions opt;
  targetMachine = std::unique_ptr<TargetMachine>(target->createTargetMachine(
      targetTriple, "generic", "", opt, Reloc::PIC_));
  context->fmodule.setDataLayout(targetMachine->createDataLayout());
#else
  auto targetTriple = sys::getDefaultTargetTriple();
  context->fmodule.setTargetTriple(targetTriple);

  std::string err;
  target = TargetRegistry::lookupTarget(targetTriple, err);
  if (!target) {
    throw std::runtime_error(
        fmt::format("failed to lookup target {}: {}", targetTriple, err));
  }

  TargetOptions opt;
  targetMachine = std::unique_ptr<TargetMachine>(target->createTargetMachine(
      targetTriple, "generic", "", opt, Reloc::PIC_));
  context->fmodule.setDataLayout(targetMachine->createDataLayout());
#endif
}

blang::Unit::Unit(Unit &&other) noexcept
    : context(std::exchange(other.context, nullptr)),
      code(std::exchange(other.code, "")),
      debug(std::exchange(other.debug, false)),
      print_ast(std::exchange(other.print_ast, false)),
      source_filename(std::exchange(other.source_filename, "")),
      build_directory(std::move(other.build_directory)),
      target(std::exchange(other.target, nullptr)),
      targetMachine(std::move(other.targetMachine)),
      optimizationLevel(std::move(other.optimizationLevel)) {
}

blang::Unit &blang::Unit::operator=(Unit &&other) noexcept {
  if (this != &other) {
    context = std::exchange(other.context, nullptr);
    code = std::exchange(other.code, "");
    debug = std::exchange(other.debug, false);
    print_ast = std::exchange(other.print_ast, false);
    source_filename = std::exchange(other.source_filename, "");
    build_directory = std::move(other.build_directory);
    targetMachine = std::move(other.targetMachine);
    optimizationLevel = std::move(other.optimizationLevel);
  }
  return *this;
}

blang::Unit::~Unit() {
  delete context;
}

void blang::Unit::compile(std::atomic<std::size_t> *success_count) {
  static std::mutex g_diagnostic_print_mutex;

  DiagnosticPrinter diag_printer = DiagnosticPrinter(source_filename, code);

  try {
    compile_uncaught();
    success_count->fetch_add(1);
  } catch (LexerException &exc) {
    std::lock_guard<std::mutex> _guard(g_diagnostic_print_mutex);
    diag_printer.printDiagnostic(exc);
  } catch (ParserException &exc) {
    std::lock_guard<std::mutex> _guard(g_diagnostic_print_mutex);
    diag_printer.printDiagnostic(exc);
  } catch (LocationException &exc) {
    std::lock_guard<std::mutex> _guard(g_diagnostic_print_mutex);
    diag_printer.printDiagnostic(exc);
  } catch (std::exception &e) {
  }
}

void blang::Unit::emit_object() {
  std::string path = fmt::format("{}/{}.o", build_directory.string(),
                                 context->fmodule.getName().str());
  std::string error;

  std::error_code EC;
  raw_fd_ostream dest = raw_fd_ostream(path, EC);
  if (EC) {
    throw std::runtime_error("could not open output file: " + EC.message());
  }

  legacy::PassManager passManager;
  auto fileType = CodeGenFileType::ObjectFile;
  if (targetMachine->addPassesToEmitFile(passManager, dest, nullptr,
                                         fileType)) {
    throw std::runtime_error("TargetMachine can't emit a file of this type");
  }

  passManager.run(context->fmodule);
  dest.flush();
}

void blang::Unit::emit_ir() {
  std::string path = fmt::format("{}/{}.ll", build_directory.string(),
                                 context->fmodule.getName().str());

  std::error_code EC;
  raw_fd_ostream dest = raw_fd_ostream(path, EC);
  if (!EC) {
    context->fmodule.print(dest, nullptr);
  }
}

void blang::Unit::emit_bindings() {
  std::string path = fmt::format("{}/{}.h", build_directory.string(),
                                 context->fmodule.getName().str());
  std::ofstream stream{path};

  Driver driver = Driver(code);
  Parser parser = Parser(driver);
  parser.parse();

  auto guard_name = "BLANG_" + context->fmodule.getName().str();
  stream << "#ifndef " << guard_name << "\n";
  stream << "#define " << guard_name << "\n";
  stream << "#include <stdint.h>\n";
  stream << "#include <stddef.h>\n";

  driver.get_root()->bindings(stream);

  stream << "#endif\n";
}

void blang::Unit::compile_uncaught() {
  if (debug)
    context->setup_debug_info(source_filename);

  Driver driver = Driver(code);
  Parser parser = Parser(driver);
  parser.parse();
  if (print_ast) {
    static std::mutex g_ast_print_mutex;
    std::lock_guard<std::mutex> _guard(g_ast_print_mutex);

    fmt::print("AST of {}:\n", source_filename);
    driver.get_root()->print();
  }
  driver.get_root()->compile(context);

  if (debug)
    context->finalize_debug_info();

  {
    /* Add compiler identification */
    Metadata *textnode = MDString::get(
        context->context, fmt::format("blang llvm {}", LLVM_VERSION_STRING));
    MDNode *IdentNode =
        MDNode::get(context->context, ArrayRef<Metadata *>(textnode));
    NamedMDNode *IdentMD =
        context->fmodule.getOrInsertNamedMetadata("llvm.ident");
    IdentMD->addOperand(IdentNode);
  }

  verifyModule(context->fmodule);

  /* Apply optimizations */
  if (optimizationLevel != llvm::OptimizationLevel::O0) {
    LoopAnalysisManager LAM;
    FunctionAnalysisManager FAM;
    CGSCCAnalysisManager CGAM;
    ModuleAnalysisManager MAM;
    PassBuilder PB;
    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);
    ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(optimizationLevel);
    MPM.run(context->fmodule, MAM);
  }

  /* Emit */
  emit_ir();
  emit_object();
  emit_bindings();
}
