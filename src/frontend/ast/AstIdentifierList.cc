#include "frontend/ast/AstIdentifierList.hh"
#include "Assert.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstIdentifierList::print(int indent) {
  printIndent(indent);
  fmt::print("- AstIdentifierList\n");
  for (auto pair : identifiers) {
    printIndent(indent + 1);
    fmt::print("- {}\n", pair.first);
  }
}

fir::Value AstIdentifierList::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  blangassert(0 && "AstIdentifierList should never be compiled");
  return {0};
}

} // namespace blang
