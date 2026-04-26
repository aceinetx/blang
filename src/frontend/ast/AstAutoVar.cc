#include "AstAutoVar.hh"
#include "Blang.hh"
#include <fmt/core.h>

namespace blang {
void AstAutoVar::print(int indent) {
  printIndent(indent);
  fmt::print("- AstAutoVar\n");
  names->print(indent + 1);
}

llvm::Value *AstAutoVar::compile(Blang *blang, bool rvalue) {
  (void)rvalue;
  for (const auto &pair : names->identifiers) {
    const auto &name = pair.first;
    const auto &location = pair.second;

    auto value =
        blang->builder.CreateAlloca(blang->get_word_ty(), nullptr, name);
    blang->add_scope_var(name, value, location);
  }

  return nullptr;
}

} // namespace blang
