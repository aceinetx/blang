#include "Blang.hh"
#include "frontend/Driver.hh"

#include "parser.tab.hpp"
#include <filesystem>
#include <fmt/core.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>

using namespace llvm;

namespace blang {
Blang::Blang(std::string moduleName) : context(moduleName) {
  InitializeNativeTarget();
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();

#if LLVM_VERSION_MAJOR > 20
  auto targetTriple = Triple(sys::getDefaultTargetTriple());

  std::string err;
  target = TargetRegistry::lookupTarget(targetTriple, err);
  if (!target) {
    throw std::runtime_error(fmt::format("failed to lookup target {}: {}",
                                         targetTriple.getTriple(), err));
  }

  context.fmodule.setTargetTriple(targetTriple);

  TargetOptions opt;
  targetMachine = std::unique_ptr<TargetMachine>(target->createTargetMachine(
      targetTriple, "generic", "", opt, Reloc::PIC_));

  context.fmodule.setDataLayout(targetMachine->createDataLayout());
#else
  auto targetTriple = sys::getDefaultTargetTriple();

  std::string err;
  target = TargetRegistry::lookupTarget(targetTriple, err);
  if (!target) {
    throw std::runtime_error(
        fmt::format("failed to lookup target {}: {}", targetTriple, err));
  }

  context.fmodule.setTargetTriple(targetTriple);

  TargetOptions opt;
  targetMachine = std::unique_ptr<TargetMachine>(target->createTargetMachine(
      targetTriple, "generic", "", opt, Reloc::PIC_));

  context.fmodule.setDataLayout(targetMachine->createDataLayout());
#endif

  context.push_scope();
}

Blang::~Blang() = default;

void Blang::compile(std::string code) {
  Driver driver = Driver(code);
  Parser parser = Parser(driver);
  parser.parse();
  driver.get_root()->print();
  driver.get_root()->compile(&context);

  {
    // Add compiler identification
    Metadata *textnode = MDString::get(
        context.context, fmt::format("blang llvm {}", LLVM_VERSION_STRING));
    MDNode *IdentNode =
        MDNode::get(context.context, ArrayRef<Metadata *>(textnode));
    NamedMDNode *IdentMD =
        context.fmodule.getOrInsertNamedMetadata("llvm.ident");
    IdentMD->addOperand(IdentNode);
  }

  verifyModule(context.fmodule);
}

void Blang::emit(std::string filename, EmitLevel level) {
  switch (level) {
  case EmitLevel::EMIT_EXE:
    [[fallthrough]];
  case EmitLevel::EMIT_OBJ: {
    std::string error;
    std::string obj_filename =
        (level == EmitLevel::EMIT_EXE) ? filename + ".o" : filename;

    std::error_code EC;
    raw_fd_ostream dest = raw_fd_ostream(obj_filename, EC);
    if (EC) {
      throw std::runtime_error("could not open output file: " + EC.message());
    }

    legacy::PassManager passManager;

    auto fileType = CodeGenFileType::ObjectFile;
    if (targetMachine->addPassesToEmitFile(passManager, dest, nullptr,
                                           fileType)) {
      throw std::runtime_error("TargetMachine can't emit a file of this type");
    }

    passManager.run(context.fmodule);
    dest.flush();

    if (level == EmitLevel::EMIT_EXE) {
      std::string cmd = fmt::format("clang {} -o {} ", obj_filename, filename);

      for (std::string lib : link_libraries)
        cmd += "-l" + lib + " ";

      for (auto path : link_paths)
        cmd += "-L" + path + " ";

      std::system(cmd.data());

      std::filesystem::remove(obj_filename);
    }

  } break;
  case EmitLevel::EMIT_IR: {
    std::error_code EC;
    raw_fd_ostream dest = raw_fd_ostream(filename, EC);
    if (EC) {
      throw std::runtime_error("could not open output file: " + EC.message());
    }

    context.fmodule.print(dest, nullptr);
  } break;
  }
}

} // namespace blang
