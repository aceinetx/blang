#include "frontend/ast/AstIf.hh"
#include "Blang.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstIf::print(int indent) {
  printIndent(indent);
  fmt::print("- AstIf\n");
  expression->print(indent + 1);
  then_node->print(indent + 1);
  if (else_node)
    else_node->print(indent + 1);
}

llvm::Value *AstIf::compile(Blang *blang, bool rvalue) {
  (void)rvalue;
  (void)blang;

  /* create the blocks */
  auto then_block =
      BasicBlock::Create(blang->context, "", blang->current_function);

  BasicBlock *else_block = nullptr;
  if (else_node)
    else_block =
        BasicBlock::Create(blang->context, "", blang->current_function);
  auto end_block =
      BasicBlock::Create(blang->context, "", blang->current_function);

  /* compile the expression */
  auto *value =
      blang->builder.CreateICmpNE(expression->compile(blang, true),
                                  ConstantInt::get(blang->get_word_ty(), 0));

  /* create the conditional branch */
  if (else_node)
    blang->builder.CreateCondBr(value, then_block, else_block);
  else
    blang->builder.CreateCondBr(value, then_block, end_block);

  /* compile the blocks */
  blang->builder.SetInsertPoint(then_block);
  then_node->compile(blang, true);
  if (!blang->builder.GetInsertBlock()->getTerminator())
    blang->builder.CreateBr(end_block);

  if (else_node) {
    blang->builder.SetInsertPoint(else_block);

    else_node->compile(blang, true);
    if (!blang->builder.GetInsertBlock()->getTerminator())
      blang->builder.CreateBr(end_block);
  }

  blang->builder.SetInsertPoint(end_block);

  return nullptr;
}

} // namespace blang
