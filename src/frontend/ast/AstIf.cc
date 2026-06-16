#include "frontend/ast/AstIf.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstIf::print(int indent) {
  printIndent(indent);
  fmt::print("- AstIf\n");
  expression->print(indent + 1);
  then_node->print(indent + 1);
  if (else_node)
    else_node->print(indent + 1);
}

fir::Value AstIf::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;

  /* create the blocks */
  auto then_block = C->ir.block();

  frIRBlock *else_block = nullptr;
  if (else_node)
    else_block = C->ir.block();
  auto end_block = C->ir.block();

  /* compile the expression */
  auto value = expression->compile(C, true);

  /* create the conditional branch */
  if (else_node)
    C->ir.cond_br(value, then_block, else_block);
  else
    C->ir.cond_br(value, then_block, end_block);

  /* compile the blocks */
  C->ir.set_insert_point(then_block);
  then_node->compile(C, true);
  C->ir.br(end_block);

  if (else_node) {
    C->ir.set_insert_point(else_block);

    else_node->compile(C, true);

    C->ir.br(end_block);
  }

  C->ir.br(end_block);

  return {0};
}

} // namespace blang
