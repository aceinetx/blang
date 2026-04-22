#include "Blang.hh"
#include "parser.tab.hpp"
#include <fmt/base.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>
#include <sstream>

using namespace llvm;

namespace blang {
Blang::Blang(std::string moduleName)
    : builder(context), fmodule(moduleName, context) {
  InitializeNativeTarget();
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();

  auto targetTriple = Triple(sys::getDefaultTargetTriple());

  std::string err;
  target = TargetRegistry::lookupTarget(targetTriple, err);
  if (!target) {
    throw std::runtime_error(fmt::format("failed to lookup target {}: {}",
                                         targetTriple.getTriple(), err));
  }

  fmodule.setTargetTriple(targetTriple);

  TargetOptions opt;
  targetMachine = std::unique_ptr<TargetMachine>(target->createTargetMachine(
      targetTriple, "generic", "", opt, Reloc::PIC_));

  fmodule.setDataLayout(targetMachine->createDataLayout());
}

Blang::~Blang() = default;

llvm::Type *Blang::getWordTy() {
  llvm::DataLayout DL = fmodule.getDataLayout();
  unsigned maxIntSize = DL.getLargestLegalIntTypeSizeInBits();
  llvm::Type *maxIntType = llvm::IntegerType::get(context, maxIntSize);
  return maxIntType;
}

void Blang::compile(std::string code) {
  Driver driver = Driver(code);
  Parser parser = Parser(driver);
  fmt::println("{}", parser.parse());
  driver.get_root()->print();
  driver.get_root()->compile(this);

  {
    // Add compiler identification
    Metadata *textnode = MDString::get(
        context, fmt::format("blang llvm {}", LLVM_VERSION_STRING));
    MDNode *IdentNode = MDNode::get(context, ArrayRef<Metadata *>(textnode));
    NamedMDNode *IdentMD = fmodule.getOrInsertNamedMetadata("llvm.ident");
    IdentMD->addOperand(IdentNode);
  }

  verifyModule(fmodule);

  std::error_code EC;

  fmodule.print(llvm::outs(), nullptr);
}
} // namespace blang
