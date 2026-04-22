#include "frontend/ast/AstNumber.hh"
#include <fmt/base.h>

namespace blang {
void AstNumber::print(int indent) {
  printIndent(indent);
  fmt::println("- AstNumber {}", number);
}

llvm::Value *AstNumber::compile() {
  return nullptr;
}

} // namespace blang
