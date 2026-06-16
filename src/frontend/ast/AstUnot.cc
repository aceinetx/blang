#include "frontend/ast/AstUnot.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

namespace blang {
void AstUnot::print(int indent) {
  printIndent(indent);
  fmt::print("- AstUnot\n");
  expression->print(indent + 1);
}

fir::Value AstUnot::compile(CompilerContext *C, bool rvalue) {
  if (!rvalue)
    throw LvalueException(location, "unary not");

  auto v = expression->compile(C, true);
  auto zero = C->ir.constant(C->get_word_ty(), 0L);
  auto one = C->ir.constant(C->get_word_ty(), 1L);

  auto isZero = C->ir.neq(v, zero);

  auto result = C->ir.select(isZero, one, zero);

  return result;
}

} // namespace blang
