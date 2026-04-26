#include "frontend/ast/AstAddrof.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

namespace blang {
void AstAddrof::print(int indent) {
  printIndent(indent);
  fmt::print("- AstAddrof\n");
  expression->print(indent + 1);
}

llvm::Value *AstAddrof::compile(CompilerContext *C, bool rvalue) {
  if (!rvalue)
    throw LvalueException(location, "addrof");

  return expression->compile(C, false);
}
} // namespace blang
