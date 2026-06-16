#include "frontend/ast/AstAutoVarList.hh"
#include "Assert.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstAutoVarList::print(int indent) {
  printIndent(indent);
  fmt::print("- AstAutoVarList\n");
  for (const auto &entry : list) {
    printIndent(indent + 1);
    fmt::print("- {}", entry.identifier);
    if (entry.initializer)
      entry.initializer->print(indent + 2);
  }
}

fir::Value AstAutoVarList::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  (void)C;
  blangassert(0 && "AstAutoVarList should not be compiled");
}

std::vector<AstAutoVarList::Entry>::iterator AstAutoVarList::begin() {
  return list.begin();
}

std::vector<AstAutoVarList::Entry>::iterator AstAutoVarList::end() {
  return list.end();
}

} // namespace blang
