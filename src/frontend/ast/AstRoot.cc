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

llvm::Value *AstRoot::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  llvm::Value *last = nullptr;
  for (auto child : children) {
    last = child->compile(C, true);
  }
  return last;
}

void AstRoot::bindings(std::ostream &os) {
  for (auto child : children) {
    child->bindings(os);
  }
}

} // namespace blang
