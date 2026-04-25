#include "frontend/ast/AstReturn.hh"
#include "Blang.hh"
#include <fmt/core.h>

namespace blang {
void AstReturn::print(int indent) {
  printIndent(indent);
  fmt::println("- AstReturn");
  expression->print(indent + 1);
}

llvm::Value *AstReturn::compile(Blang *blang, bool rvalue) {
  (void)rvalue;
  blang->builder.CreateRet(expression->compile(blang, true));
  return nullptr;
}
} // namespace blang
