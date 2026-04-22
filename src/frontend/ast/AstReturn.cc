#include "frontend/ast/AstReturn.hh"
#include <fmt/base.h>

namespace blang {
void AstReturn::print(int indent) {
  printIndent(indent);
  fmt::println("- AstReturn");
  expression->print(indent + 1);
}
llvm::Value *AstReturn::compile() {
  return expression->compile();
}
} // namespace blang
