#include "frontend/ast/AstReturn.hh"
#include "Blang.hh"
#include <fmt/base.h>

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

bool AstReturn::is_rvalue() {
  return true;
}
} // namespace blang
