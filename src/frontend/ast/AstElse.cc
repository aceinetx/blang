#include "frontend/ast/AstElse.hh"
#include "Assert.hh"
#include "Blang.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstElse::print(int indent) {
  printIndent(indent);
  fmt::println("- AstElse");
  for (auto statement : body) {
    statement->print(indent + 1);
  }
}

llvm::Value *AstElse::compile(Blang *blang, bool rvalue) {
  (void)rvalue;
  (void)blang;
  blangassert(0 && "AstElse should never be compiled");
  return nullptr;
}

} // namespace blang
