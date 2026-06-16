#include "frontend/ast/AstTernary.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstTernary::print(int indent) {
  printIndent(indent);
  fmt::print("- AstTernary\n");
  expression->print(indent + 1);
  true_expression->print(indent + 1);
  false_expression->print(indent + 1);
}

fir::Value AstTernary::compile(CompilerContext *C, bool rvalue) {
  auto test = expression->compile(C, true);
  auto truev = true_expression->compile(C, true);
  auto falsev = false_expression->compile(C, true);
  return C->ir.select(test, truev, falsev);
}

} // namespace blang
