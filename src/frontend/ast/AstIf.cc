#include "frontend/ast/AstIf.hh"
#include "CompilerContext.hh"
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

llvm::Value *AstIf::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;

  /* create the blocks */
  auto then_block = BasicBlock::Create(C->context, "", C->current_function);

  BasicBlock *else_block = nullptr;
  if (else_node)
    else_block = BasicBlock::Create(C->context, "", C->current_function);
  auto end_block = BasicBlock::Create(C->context, "", C->current_function);

  /* compile the expression */
  auto *value = C->builder.CreateICmpNE(expression->compile(C, true),
                                        ConstantInt::get(C->get_word_ty(), 0));

  /* create the conditional branch */
  if (else_node)
    C->builder.CreateCondBr(value, then_block, else_block);
  else
    C->builder.CreateCondBr(value, then_block, end_block);

  /* compile the blocks */
  C->builder.SetInsertPoint(then_block);
  then_node->compile(C, true);
  if (!C->builder.GetInsertBlock()->getTerminator())
    C->builder.CreateBr(end_block);

  if (else_node) {
    C->builder.SetInsertPoint(else_block);

    else_node->compile(C, true);
    if (!C->builder.GetInsertBlock()->getTerminator())
      C->builder.CreateBr(end_block);
  }

  C->builder.SetInsertPoint(end_block);

  return nullptr;
}

} // namespace blang
