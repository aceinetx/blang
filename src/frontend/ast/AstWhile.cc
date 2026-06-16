#include "frontend/ast/AstWhile.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstWhile::print(int indent) {
  printIndent(indent);
  fmt::print("- AstWhile\n");
  block->print(indent + 1);
}

fir::Value AstWhile::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;

  auto comparison_block = C->ir.block();
  auto body_block = C->ir.block();
  auto end_block = C->ir.block();
  C->while_statement_end_block = end_block;

  C->ir.br(comparison_block);
  C->ir.set_insert_point(comparison_block);
  auto value = expression->compile(C, true);
  auto zero = C->ir.constant(C->get_word_ty(), 0L);
  value = C->ir.neq(value, zero);
  C->ir.cond_br(value, body_block, end_block);

  C->ir.set_insert_point(body_block);
  block->compile(C, true);
  C->ir.br(comparison_block);
  C->ir.set_insert_point(end_block);

  return {0};
}

} // namespace blang
