#include "AstAutoVar.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstAutoVar::print(int indent) {
  printIndent(indent);
  fmt::print("- AstAutoVar\n");
  names->print(indent + 1);
}

llvm::Value *AstAutoVar::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  for (const auto &pair : names->identifiers) {
    const auto &name = pair.first;
    const auto &location = pair.second;

    auto value =
        C->builder.CreateAlloca(C->get_word_ty(), nullptr, name);
    C->add_scope_var(name, value, location);
  }

  return nullptr;
}

} // namespace blang
