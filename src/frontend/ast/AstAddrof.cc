#include "frontend/ast/AstAddrof.hh"
#include "Assert.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstAddrof::print(int indent) {
  printIndent(indent);
  fmt::println("- AstAddrof");
  expression->print(indent + 1);
}

llvm::Value *AstAddrof::compile(Blang *blang, bool rvalue) {
  blangassert(rvalue);
  return expression->compile(blang, false);
}
} // namespace blang
