#include "frontend/ast/AstBlock.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstBlock::print(int indent) {
  printIndent(indent);
  fmt::print("- AstBlock\n");
  for (auto child : children)
    child->print(indent + 1);
}

fir::Value AstBlock::compile(CompilerContext *C, bool rvalue) {
  fir::Value last;
  for (auto child : children)
    last = child->compile(C, rvalue);
  return last;
}

} // namespace blang
