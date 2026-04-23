#include "frontend/ast/AstVarRef.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstVarRef::print(int indent) {
  printIndent(indent);
  fmt::println("- AstVarRef {}", name);
}

llvm::Value *AstVarRef::compile(Blang *blang) {
  assert(name != "");

  return blang->get_scope_var(name);
}

} // namespace blang
