#include "frontend/ast/AstElseIf.hh"
#include "Assert.hh"
#include "Blang.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void AstElseIf::print(int indent) {
  printIndent(indent);
  fmt::println("- AstElseIf");
  expression->print(indent + 1);
  for (auto statement : body) {
    statement->print(indent + 1);
  }
}

llvm::Value *AstElseIf::compile(Blang *blang, bool rvalue) {
  (void)rvalue;
  (void)blang;
  blangassert(0 && "AstElseIf should never be compiled");
  return nullptr;
}

} // namespace blang
