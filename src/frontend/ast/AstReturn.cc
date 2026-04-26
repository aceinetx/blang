#include "frontend/ast/AstReturn.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstReturn::print(int indent) {
  printIndent(indent);
  fmt::print("- AstReturn\n");
  expression->print(indent + 1);
}

llvm::Value *AstReturn::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  C->builder.CreateRet(expression->compile(C, true));
  return nullptr;
}
} // namespace blang
