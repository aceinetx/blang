#include "AstBinop.hh"
#include "Assert.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstBinop::print(int indent) {
  printIndent(indent);
  fmt::print("- AstBinop {}\n", (int)op);

  left->print(indent + 1);
  right->print(indent + 1);
}

fir::Value AstBinop::compile(CompilerContext *C, bool rvalue) {
  blangassert(rvalue);
  auto lhs = left->compile(C, true);
  auto rhs = right->compile(C, true);
  fir::Value result;

  switch (op) {
  case PLUS:
    result = C->ir.add(lhs, rhs);
    break;
  case MINUS:
    result = C->ir.sub(lhs, rhs);
    break;
  case MUL:
    result = C->ir.mul(lhs, rhs);
    break;
  case DIV:
    result = C->ir.div(lhs, rhs);
    break;
  case EQUAL:
    result = C->ir.eq(lhs, rhs);
    break;
  case NEQUAL:
    result = C->ir.neq(lhs, rhs);
    break;
  case GREATER:
    result = C->ir.gt(lhs, rhs);
    break;
  case LESS:
    result = C->ir.lt(lhs, rhs);
    break;
  case GREQ:
    result = C->ir.gte(lhs, rhs);
    break;
  case LSEQ:
    result = C->ir.lte(lhs, rhs);
    break;
  case BITOR:
    result = C->ir.bit_or(lhs, rhs);
    break;
  case BITAND:
    result = C->ir.bit_and(lhs, rhs);
    break;
  case BITSHL:
    result = C->ir.bit_shl(lhs, rhs);
    break;
  case BITSHR:
    result = C->ir.bit_shr(lhs, rhs);
    break;
  case PERCENT:
    blangassert(0 && "Unsupported");
    break;
  }

  return result;
}

} // namespace blang
