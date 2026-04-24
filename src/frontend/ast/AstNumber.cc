#include "frontend/ast/AstNumber.hh"
#include "Blang.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void AstNumber::print(int indent) {
  printIndent(indent);
  fmt::println("- AstNumber {}", number);
}

llvm::Value *AstNumber::compile(Blang *blang, bool rvalue) {
  if (!rvalue) {
    throw std::runtime_error("number cannot be an lvalue");
  }

  return ConstantInt::get(blang->get_word_ty(), number);
}
} // namespace blang
