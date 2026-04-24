#include "frontend/ast/AstNumber.hh"
#include "Blang.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void AstNumber::print(int indent) {
  printIndent(indent);
  fmt::println("- AstNumber {}", number);
}

llvm::Value *AstNumber::compile(Blang *blang, bool rvalue) {
  if (!rvalue) {
    throw LvalueException(location, "number");
  }

  return ConstantInt::get(blang->get_word_ty(), number);
}
} // namespace blang
