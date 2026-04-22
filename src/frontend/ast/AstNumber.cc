#include "frontend/ast/AstNumber.hh"
#include "Blang.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void AstNumber::print(int indent) {
  printIndent(indent);
  fmt::println("- AstNumber {}", number);
}

llvm::Value *AstNumber::compile(Blang *blang) {
  return ConstantInt::get(blang->getWordTy(), number);
}

} // namespace blang
