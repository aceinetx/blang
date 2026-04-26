#include "frontend/ast/AstBlock.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstBlock::print(int indent) {
  printIndent(indent);
  fmt::println("- AstBlock");
  for (auto child : children)
    child->print(indent + 1);
}

llvm::Value *AstBlock::compile(CompilerContext *C, bool rvalue) {
  Value *last = nullptr;
  for (auto child : children)
    last = child->compile(C, rvalue);
  return last;
}

} // namespace blang
