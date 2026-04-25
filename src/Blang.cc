#include "Blang.hh"
#include "frontend/Driver.hh"

#include "frontend/exceptions/RedefinitionException/RedefinitionException.hh"
#include "parser.tab.hpp"
#include <fmt/core.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>

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

#if LLVM_VERSION_MAJOR > 20
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
#else
  auto targetTriple = sys::getDefaultTargetTriple();

  std::string err;
  target = TargetRegistry::lookupTarget(targetTriple, err);
  if (!target) {
    throw std::runtime_error(
        fmt::format("failed to lookup target {}: {}", targetTriple, err));
  }

  fmodule.setTargetTriple(targetTriple);

  TargetOptions opt;
  targetMachine = std::unique_ptr<TargetMachine>(target->createTargetMachine(
      targetTriple, "generic", "", opt, Reloc::PIC_));

  fmodule.setDataLayout(targetMachine->createDataLayout());
#endif

  push_scope();
}

Blang::~Blang() = default;

llvm::Type *Blang::get_word_ty() {
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

void Blang::push_scope() {
  scopes.insert(scopes.begin(), Scope());
}

void Blang::pop_scope() {
  scopes.erase(scopes.begin());
}

Scope &Blang::get_scope() {
  return scopes[0];
}

llvm::Value *Blang::get_scope_var(std::string name) {
  for (auto &scope : scopes) {
    if (scope.contains(name))
      return scope[name];
  }
  return nullptr;
}

void Blang::add_scope_var(std::string name, llvm::Value *value,
                          std::optional<class location> diagnostic_location) {
  auto &scope = get_scope();
  if (scope.contains(name)) {
    if (diagnostic_location)
      throw RedefinitionException(*diagnostic_location, name);
    throw std::runtime_error("redefinition of " + name);
  }
  scope[name] = value;
}

void Blang::add_global_scope_var(
    std::string name, llvm::Value *value,
    std::optional<class location> diagnostic_location) {
  auto &scope = scopes.back();
  if (scope.contains(name)) {
    if (diagnostic_location)
      throw RedefinitionException(*diagnostic_location, "global " + name);
    throw std::runtime_error("redefinition of global " + name);
  }
  scope[name] = value;
}

} // namespace blang
