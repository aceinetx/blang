#include "frontend/ast/AstIncDec.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

namespace blang {
void AstIncDec::print(int indent) {
  printIndent(indent);
  fmt::print("- AstIncDec {} {}\n", type, op);
  expression->print(indent + 1);
}

fir::Value AstIncDec::compile(CompilerContext *C, bool rvalue) {
  if (!rvalue) {
    throw LvalueException(location, "unary incdec");
  }

  auto one = C->ir.constant(C->get_word_ty(), 1L);
  auto value = expression->compile(C, false);
  value = C->ir.cast(value, C->get_word_ptr_ty());

  fir::Value output;

  if (type == POST)
    output = C->ir.load(value, C->get_word_ty());

  if (op == INC) {
    auto new_value = C->ir.load(value, C->get_word_ty());
    new_value = C->ir.add(new_value, one);
    C->ir.store(value, new_value);
  } else if (op == DEC) {
    auto new_value = C->ir.load(value, C->get_word_ty());
    new_value = C->ir.sub(new_value, one);
    C->ir.store(value, new_value);
  }

  if (type == PRE)
    output = C->ir.load(value, C->get_word_ty());

  return output;
}

} // namespace blang
