#include "frontend/ast/AstFuncDef.hh"
#include <fmt/base.h>

namespace blang {
void AstFuncDef::print(int indent) {
  printIndent(indent);
  fmt::println("- AstFuncDef");
  for (auto child : statements) {
    child->print(indent + 1);
  }
}

llvm::Value *AstFuncDef::compile() {
  llvm::Value *last = nullptr;
  for (auto child : statements) {
    last = child->compile();
  }
  return last;
}
} // namespace blang
