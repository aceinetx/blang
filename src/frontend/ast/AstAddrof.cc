#include "frontend/ast/AstAddrof.hh"
#include "Blang.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/base.h>

namespace blang {
void AstAddrof::print(int indent) {
  printIndent(indent);
  fmt::println("- AstAddrof");
  expression->print(indent + 1);
}

llvm::Value *AstAddrof::compile(Blang *blang, bool rvalue) {
  if (!rvalue)
    throw LvalueException(location, "addrof");

  return expression->compile(blang, false);
}
} // namespace blang
