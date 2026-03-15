#include "blang.hh"
#include "bison/lexer.hh"

using namespace blang;
using namespace llvm;

Scope::Scope() {
  variables.clear();
}

Blang::Blang(std::string moduleName)
    : builder(context), fmodule(moduleName, context), ifID(0), whileID(0) {
  InitializeNativeTarget();
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();

  std::string TargetTriple = sys::getDefaultTargetTriple();

  std::string err;
  target = (Target *)TargetRegistry::lookupTarget(TargetTriple, err);
  if (!target) {
    std::cerr << "blang: error: failed to lookup target " + TargetTriple +
                     ": " + err;
    return;
  }

  Triple triple = Triple(TargetTriple);
  fmodule.setTargetTriple(triple);

  TargetOptions opt;
  targetMachine =
      target->createTargetMachine(triple, "generic", "", opt, Reloc::PIC_);

  fmodule.setDataLayout(targetMachine->createDataLayout());
}

Blang::~Blang() {
  if (targetMachine)
    delete targetMachine;
}

Result<NoSuccess, std::string> Blang::parseAndCompile() {
  scopes.clear();
  scopes.push_back({});

  std::stringstream ss(input);
  std::stringstream os;

  yy::Lexer lexer;
  lexer.switch_streams(ss, os);
  auto root = std::make_shared<AstRootNode>();
  parser = std::make_unique<yy::Parser>(lexer, root);

  // TODO: handle errors here
  /*
if (!parser->root) {
return Result<NoSuccess, std::string>::error(
  fmt::format("parse error: {}", parser->error));
}

if (!parser->root->compile(this)) {
return Result<NoSuccess, std::string>::error(
  fmt::format("compile error: {}", compile_error));
}
  */

  {
    Metadata *textnode = MDString::get(
        context, fmt::format("blang llvm {}", LLVM_VERSION_STRING));
    MDNode *IdentNode = MDNode::get(context, ArrayRef<Metadata *>(textnode));
    NamedMDNode *IdentMD = fmodule.getOrInsertNamedMetadata("llvm.ident");
    IdentMD->addOperand(IdentNode);
  }

  verifyModule(fmodule);

  return Result<NoSuccess, std::string>::success({});
}

Variable *Blang::getVariable(std::string name) {
  for (Scope &scope : scopes) {
    for (std::unique_ptr<Variable> &var : scope.variables) {
      if (var->name != name)
        continue;

      return var.get();
    }
  }

  if (extern_symbols.count(name)) {
    ExternSymbol &ext = extern_symbols[name];
    ext.isFunction = false;

    if (!ext.isExterned) {
      GlobalVariable *GV =
          new GlobalVariable(fmodule, getBWordTy(), false,
                             GlobalValue::ExternalLinkage, nullptr, name);
      auto var = std::make_unique<Variable>();
      var->value = GV;
      var->name = name;
      scopes.begin()->variables.push_back(std::move(var));

      ext.isExterned = true;

      return scopes.begin()->variables.back().get();
    }
  }

  return nullptr;
}

Type *Blang::getBWordTy() {
  DataLayout DL = fmodule.getDataLayout();
  // unsigned LargestInt = DL.getLargestLegalIntTypeSizeInBits();
  unsigned maxIntSize = DL.getLargestLegalIntTypeSizeInBits();
  Type *maxIntType = IntegerType::get(context, maxIntSize);
  return maxIntType;
}

Result<NoSuccess, std::string> Blang::emit(std::string filename,
                                           EmitLevel level) {
#define ERROR(...)                                                             \
  return Result<NoSuccess, std::string>::error(fmt::format(__VA_ARGS__))
#define SUCCESS(...) return Result<NoSuccess, std::string>::success({})

  switch (level) {
  case EmitLevel::EMIT_EXE:
    [[fallthrough]];
  case EmitLevel::EMIT_OBJ: {
    std::string error;
    std::string obj_filename =
        (level == EmitLevel::EMIT_EXE) ? filename + ".o" : filename;

    std::error_code EC;
    raw_fd_ostream dest(obj_filename, EC, sys::fs::OF_None);
    if (EC) {
      ERROR("could not open output file: {}", EC.message());
    }

    legacy::PassManager pass;
    auto fileType = CodeGenFileType::ObjectFile;
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
      ERROR("TargetMachine can't emit a file of this type");
    }

    pass.run(fmodule);
    dest.flush();

    if (level == EmitLevel::EMIT_EXE) {
      std::string cmd = fmt::format("clang {} -o {} ", obj_filename, filename);

      for (std::string lib : link_libraries)
        cmd += "-l" + lib + " ";

      if (!link_path.empty())
        cmd += "-L" + link_path + " ";
      if (libb)
        cmd += "-lb -nostdlib -lc /usr/lib/brt.o ";

      std::system(cmd.data());

      std::error_code err = sys::fs::remove(obj_filename);
      (void)err;
    }

    SUCCESS();
  } break;
  case EmitLevel::EMIT_IR: {
    std::error_code EC;
    raw_fd_ostream dest(filename, EC, sys::fs::OF_None);
    if (EC) {
      ERROR("could not open output file: {}", EC.message());
    }

    fmodule.print(dest, nullptr);
    SUCCESS();
  } break;
  }

  ERROR("unreachable");
#undef ERROR
#undef SUCCESS
}
