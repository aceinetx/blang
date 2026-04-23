#include "frontend/ast/AstVarRef.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstVarRef::print(int indent) {
  printIndent(indent);
  fmt::println("- AstVarRef {}", name);
}

llvm::Value *AstVarRef::compile(Blang *blang, bool rvalue) {
  assert(name != "");

  auto *value = blang->get_scope_var(name);
  return rvalue ? blang->builder.CreateLoad(blang->get_word_ty(), value)
                : value;
}

bool AstVarRef::is_rvalue() {
  return false;
}
} // namespace blang
