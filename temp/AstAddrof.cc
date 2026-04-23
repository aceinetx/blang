#include "frontend/ast/AstAddrof.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstAddrof::print(int indent) {
  printIndent(indent);
  fmt::println("- AstAddrof");
  expression->print(indent + 1);
}

llvm::Value *AstAddrof::compile(Blang *blang) {
  return expression->compile(blang);
}

} // namespace blang
