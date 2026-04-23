#include "frontend/ast/AstIndex.hh"
#include "Assert.hh"
#include "Blang.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void AstIndex::print(int indent) {
  printIndent(indent);
  fmt::println("- AstIndex");
  expression->print(indent + 1);
  index->print(indent + 1);
}

llvm::Value *AstIndex::compile(Blang *blang) {
  blangassert(0 && "AstIndex::compile is not implemented");
  return nullptr;
}

} // namespace blang
