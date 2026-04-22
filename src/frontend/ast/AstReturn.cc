#include "frontend/ast/AstReturn.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstReturn::print(int indent) {
  printIndent(indent);
  fmt::println("- AstReturn");
  expression->print(indent + 1);
}

llvm::Value *AstReturn::compile(Blang *blang) {
  blang->builder.CreateRet(expression->compile(blang));
  return nullptr;
}
} // namespace blang
