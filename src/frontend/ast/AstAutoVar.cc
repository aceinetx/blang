#include "AstAutoVar.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstAutoVar::print(int indent) {
  printIndent(indent);
  fmt::println("- AstAutoVar {}", name);
}

llvm::Value *AstAutoVar::compile(Blang *blang, bool rvalue) {
  (void)rvalue;
  assert(name != "");

  auto value = blang->builder.CreateAlloca(blang->get_word_ty(), nullptr, name);
  blang->add_scope_var(name, value, identifier_location);

  return nullptr;
}

} // namespace blang
