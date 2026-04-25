#include "frontend/ast/AstIf.hh"
#include "Assert.hh"
#include "Blang.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstIf::print(int indent) {
  printIndent(indent);
  fmt::print("- AstIf\n");
  expression->print(indent + 1);
  body->print(indent + 1);
}

llvm::Value *AstIf::compile(Blang *blang, bool rvalue) {
  (void)rvalue;
  (void)blang;
  blangassert(0 && "AstIf should never be compiled");
  return nullptr;
}

} // namespace blang
