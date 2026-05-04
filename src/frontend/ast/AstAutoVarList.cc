#include "frontend/ast/AstAutoVarList.hh"
#include "Assert.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

using namespace llvm;

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

llvm::Value *AstAutoVarList::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  (void)C;
  blangassert(0 && "AstAutoVarList should not be compiled");
  return nullptr;
}

std::vector<AstAutoVarList::Entry>::iterator AstAutoVarList::begin() {
  return list.begin();
}

std::vector<AstAutoVarList::Entry>::iterator AstAutoVarList::end() {
  return list.end();
}

} // namespace blang
