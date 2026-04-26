#include "frontend/ast/AstSwitch.hh"
#include "Blang.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstSwitch::print(int indent) {
  printIndent(indent);
  fmt::println("- AstSwitch");
  expression->print(indent + 1);
  statement->print(indent + 1);
}

llvm::Value *AstSwitch::compile(Blang *blang, bool rvalue) {
  blang->last_switch = this;

  value = expression->compile(blang, true);

  evaluator = BasicBlock::Create(blang->context, "", blang->current_function);
  body = BasicBlock::Create(blang->context, "", blang->current_function);
  end = BasicBlock::Create(blang->context, "", blang->current_function);

  blang->builder.CreateBr(evaluator);

  blang->builder.SetInsertPoint(body);
  statement->compile(blang, true);

  if (!blang->builder.GetInsertBlock()->getTerminator())
    blang->builder.CreateBr(end);

  blang->builder.SetInsertPoint(evaluator);
  if (!evaluator->getTerminator())
    blang->builder.CreateBr(body);

  blang->builder.SetInsertPoint(end);

  blang->last_switch = nullptr;
  return nullptr;
}

void AstSwitch::add_case(Blang *blang, long number, llvm::BasicBlock *block) {
  blang->builder.SetInsertPoint(evaluator);
  auto result = blang->builder.CreateICmpEQ(
      value, ConstantInt::get(blang->get_word_ty(), number));

  BasicBlock *new_evaluator =
      BasicBlock::Create(blang->context, "", blang->current_function);

  blang->builder.CreateCondBr(result, block, new_evaluator);

  evaluator = new_evaluator;
}
} // namespace blang
