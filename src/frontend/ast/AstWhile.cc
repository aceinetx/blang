#include "frontend/ast/AstWhile.hh"
#include "Blang.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstWhile::print(int indent) {
  printIndent(indent);
  fmt::print("- AstWhile\n");
  for (auto statement : statements) {
    statement->print(indent + 1);
  }
}

llvm::Value *AstWhile::compile(Blang *blang, bool rvalue) {
  (void)rvalue;

  llvm::BasicBlock *comparison_block =
      llvm::BasicBlock::Create(blang->context, "", blang->current_function);
  llvm::BasicBlock *body_block =
      llvm::BasicBlock::Create(blang->context, "", blang->current_function);
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(blang->context, "", blang->current_function);

  blang->builder.CreateBr(comparison_block);
  blang->builder.SetInsertPoint(comparison_block);
  auto *value =
      blang->builder.CreateICmpNE(expression->compile(blang, true),
                                  ConstantInt::get(blang->get_word_ty(), 0));
  blang->builder.CreateCondBr(value, body_block, end_block);

  blang->builder.SetInsertPoint(body_block);
  for (auto statement : statements) {
    statement->compile(blang, true);
  }
  if (!blang->builder.GetInsertBlock()->getTerminator()) {
    blang->builder.CreateBr(comparison_block);
  }
  blang->builder.SetInsertPoint(end_block);

  return nullptr;
}

} // namespace blang
