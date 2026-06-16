#include "frontend/ast/AstUinv.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

namespace blang {
void AstUinv::print(int indent) {
  printIndent(indent);
  fmt::print("- AstUinv\n");
  expression->print(indent + 1);
}

fir::Value AstUinv::compile(CompilerContext *C, bool rvalue) {
  if (!rvalue) {
    throw LvalueException(location, "unary inverse");
  }

  auto value = expression->compile(C, true);
  auto zero = C->ir.constant(C->get_word_ty(), 0L);
  auto result = C->ir.sub(value, zero);

  return result;
}

} // namespace blang
