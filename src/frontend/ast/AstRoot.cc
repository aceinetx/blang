#include "frontend/ast/AstRoot.hh"
#include <fmt/core.h>

namespace blang {
void AstRoot::print(int indent) {
  printIndent(indent);
  fmt::print("- AstRoot\n");
  for (auto child : children) {
    child->print(indent + 1);
  }
}

llvm::Value *AstRoot::compile(Blang *blang, bool rvalue) {
  (void)rvalue;
  llvm::Value *last = nullptr;
  for (auto child : children) {
    last = child->compile(blang, true);
  }
  return last;
}
} // namespace blang
