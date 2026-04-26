#include "frontend/ast/AstExtern.hh"
#include "Blang.hh"
#include <fmt/core.h>
#include <llvm/IR/GlobalVariable.h>

using namespace llvm;

namespace blang {
AstExtern::AstExtern() : names(std::make_shared<AstIdentifierList>()) {
}

void AstExtern::print(int indent) {
  printIndent(indent);
  fmt::print("- AstExtern\n");
  names->print(indent + 1);
  fmt::print("\n");
}

llvm::Value *AstExtern::compile(Blang *blang, bool rvalue) {
  (void)rvalue;
  for (const auto &pair : names->identifiers) {
    const auto &name = pair.first;
    const auto &location = pair.second;

    if (!blang->extern_values.contains(name)) {
      auto *GV =
          new GlobalVariable(blang->fmodule, blang->get_word_ty(), false,
                             GlobalValue::ExternalLinkage, nullptr, name);
      blang->extern_values[name] = GV;
      blang->add_scope_var(name, GV, location);
    } else {
      blang->add_scope_var(name, blang->extern_values[name], location);
    }
  }

  return nullptr;
}
} // namespace blang
