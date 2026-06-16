#include "frontend/ast/AstSwitch.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstSwitch::print(int indent) {
  printIndent(indent);
  fmt::print("- AstSwitch\n");
  expression->print(indent + 1);
  statement->print(indent + 1);
}

fir::Value AstSwitch::compile(CompilerContext *C, bool rvalue) {
  C->last_switch = this;

  value = expression->compile(C, true);

  evaluator = C->ir.block();
  body = C->ir.block();
  end = C->ir.block();

  C->ir.br(evaluator);

  C->ir.set_insert_point(body);
  statement->compile(C, true);

  C->ir.br(end);

  C->ir.set_insert_point(evaluator);
  C->ir.br(end);

  C->ir.set_insert_point(end);

  C->last_switch = nullptr;
  return {0};
}

void AstSwitch::add_case(CompilerContext *C, long number, fir::Block block) {
  C->ir.set_insert_point(evaluator);
  auto number_value = C->ir.constant(C->get_word_ty(), number);
  auto result = C->ir.neq(value, number_value);

  auto new_evaluator = C->ir.block();

  C->ir.cond_br(result, new_evaluator, block);

  evaluator = new_evaluator;
}
} // namespace blang
