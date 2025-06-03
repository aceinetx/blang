#include "blang.hh"
#include <parser.hh>

typedef void *yyscan_t;
void yylex_init(yyscan_t *);
void yylex_destroy(yyscan_t);
void yyset_extra(blang::Parser *user_defined, void *yyscanner);

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

  TargetOptions opt;
  targetMachine = target->createTargetMachine(TargetTriple, "generic", "", opt,
                                              Reloc::PIC_);

  fmodule.setTargetTriple(TargetTriple);
  fmodule.setDataLayout(targetMachine->createDataLayout());

  parser = new Parser();
}

Blang::~Blang() {
  if (parser)
    delete parser;
  if (targetMachine)
    delete targetMachine;
}

Result<NoSuccess, std::string> Blang::parseAndCompile() {
  scopes.clear();
  scopes.push_back({});

  yyscan_t scanner;
  YY_BUFFER_STATE buffer;

  yylex_init(&scanner);
  yyset_extra(this->parser, scanner);

  buffer = yy_scan_string(input.c_str(), scanner);
  yyparse(scanner, this->parser);

  if (!parser->root) {
    return Result<NoSuccess, std::string>::error(
        fmt::format("parse error: {}", parser->error));
  }

  yy_delete_buffer(buffer, scanner);
  yylex_destroy(scanner);

  if (!parser->root->compile(this)) {
    delete parser->root;
    return Result<NoSuccess, std::string>::error(
        fmt::format("compile error: {}", compile_error));
  }

  delete parser->root;

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
      scopes.back().variables.push_back(std::move(var));

      ext.isExterned = true;

      return scopes.back().variables.back().get();
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
      std::string clang_path;
      if (auto Path = sys::findProgramByName("clang")) {
        clang_path = *Path;
      } else {
        ERROR("clang not found in path");
      }

      std::vector<StringRef> linker_args = {clang_path, obj_filename, "-o",
                                            filename};
      std::string linker_error;
      sys::ExecuteAndWait(linker_args[0], linker_args,
                          std::optional<ArrayRef<StringRef>>(), {}, 0, 0,
                          &linker_error);

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
