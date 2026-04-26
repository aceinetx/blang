#include "frontend/ast/AstWhile.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstWhile::print(int indent) {
  printIndent(indent);
  fmt::print("- AstWhile\n");
  block->print(indent + 1);
}

llvm::Value *AstWhile::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;

  llvm::BasicBlock *comparison_block =
      llvm::BasicBlock::Create(C->context, "", C->current_function);
  llvm::BasicBlock *body_block =
      llvm::BasicBlock::Create(C->context, "", C->current_function);
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(C->context, "", C->current_function);
  C->while_statement_end_block = end_block;

  C->builder.CreateBr(comparison_block);
  C->builder.SetInsertPoint(comparison_block);
  auto *value =
      C->builder.CreateICmpNE(expression->compile(C, true),
                                  ConstantInt::get(C->get_word_ty(), 0));
  C->builder.CreateCondBr(value, body_block, end_block);

  C->builder.SetInsertPoint(body_block);
  block->compile(C, true);
  if (!C->builder.GetInsertBlock()->getTerminator()) {
    C->builder.CreateBr(comparison_block);
  }
  C->builder.SetInsertPoint(end_block);

  return nullptr;
}

} // namespace blang
