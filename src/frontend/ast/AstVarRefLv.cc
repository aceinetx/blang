#include "frontend/ast/AstVarRefLv.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstVarRefLv::print(int indent) {
  printIndent(indent);
  fmt::println("- AstVarRefLv {}", name);
}

llvm::Value *AstVarRefLv::compile(Blang *blang) {
  assert(name != "");

  return blang->get_scope_var(name);
}

} // namespace blang
