#include "frontend/ast/AstVarRefRv.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstVarRefRv::print(int indent) {
  printIndent(indent);
  fmt::println("- AstVarRefRv {}", name);
}

llvm::Value *AstVarRefRv::compile(Blang *blang) {
  assert(name != "");

  return blang->builder.CreateLoad(blang->get_word_ty(),
                                   blang->get_scope_var(name));
}

} // namespace blang
