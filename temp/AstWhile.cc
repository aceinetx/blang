#include "frontend/ast/AstWhile.hh"
#include "Blang.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void AstWhile::print(int indent) {
  printIndent(indent);
  fmt::println("- AstWhile");
  for (auto statement : statements) {
    statement->print(indent + 1);
  }
}

llvm::Value *AstWhile::compile(Blang *blang) {
  llvm::BasicBlock *comparison_block =
      llvm::BasicBlock::Create(blang->context, "", blang->current_function);
  llvm::BasicBlock *body_block =
      llvm::BasicBlock::Create(blang->context, "", blang->current_function);
  llvm::BasicBlock *end_block =
      llvm::BasicBlock::Create(blang->context, "", blang->current_function);

  blang->builder.CreateBr(comparison_block);
  blang->builder.SetInsertPoint(comparison_block);
  auto *value = blang->builder.CreateICmpNE(
      expression->compile(blang), ConstantInt::get(blang->get_word_ty(), 0));
  blang->builder.CreateCondBr(value, body_block, end_block);

  blang->builder.SetInsertPoint(body_block);
  for (auto statement : statements) {
    statement->compile(blang);
  }
  if (!blang->builder.GetInsertBlock()->getTerminator()) {
    blang->builder.CreateBr(comparison_block);
  }
  blang->builder.SetInsertPoint(end_block);

  return nullptr;
}

} // namespace blang
