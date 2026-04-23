#include "frontend/ast/AstIf.hh"
#include "Assert.hh"
#include "Blang.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void AstIf::print(int indent) {
  printIndent(indent);
  fmt::println("- AstIf");
  expression->print(indent + 1);
  for (auto statement : body) {
    statement->print(indent + 1);
  }
}

llvm::Value *AstIf::compile(Blang *blang) {
  blangassert(0 && "AstIf should never be compiled");
  return nullptr;
}

} // namespace blang
