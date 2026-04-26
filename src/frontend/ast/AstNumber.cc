#include "frontend/ast/AstNumber.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstNumber::print(int indent) {
  printIndent(indent);
  fmt::print("- AstNumber {}\n", number);
}

llvm::Value *AstNumber::compile(CompilerContext *C, bool rvalue) {
  if (!rvalue) {
    throw LvalueException(location, "number");
  }

  return ConstantInt::get(C->get_word_ty(), number);
}
} // namespace blang
