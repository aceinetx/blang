#include "frontend/ast/AstNumber.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

namespace blang {
void AstNumber::print(int indent) {
  printIndent(indent);
  fmt::print("- AstNumber {}\n", number);
}

fir::Value AstNumber::compile(CompilerContext *C, bool rvalue) {
  if (!rvalue) {
    throw LvalueException(location, "number");
  }

  return C->ir.constant(C->get_word_ty(), number);
}
} // namespace blang
