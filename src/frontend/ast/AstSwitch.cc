#include "frontend/ast/AstSwitch.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstSwitch::print(int indent) {
  printIndent(indent);
  fmt::println("- AstSwitch");
  expression->print(indent + 1);
  statement->print(indent + 1);
}

llvm::Value *AstSwitch::compile(CompilerContext *C, bool rvalue) {
  C->last_switch = this;

  value = expression->compile(C, true);

  evaluator = BasicBlock::Create(C->context, "", C->current_function);
  body = BasicBlock::Create(C->context, "", C->current_function);
  end = BasicBlock::Create(C->context, "", C->current_function);

  C->builder.CreateBr(evaluator);

  C->builder.SetInsertPoint(body);
  statement->compile(C, true);

  if (!C->builder.GetInsertBlock()->getTerminator())
    C->builder.CreateBr(end);

  C->builder.SetInsertPoint(evaluator);
  if (!evaluator->getTerminator())
    C->builder.CreateBr(end);

  C->builder.SetInsertPoint(end);

  C->last_switch = nullptr;
  return nullptr;
}

void AstSwitch::add_case(CompilerContext *C, long number, llvm::BasicBlock *block) {
  C->builder.SetInsertPoint(evaluator);
  auto result = C->builder.CreateICmpEQ(
      value, ConstantInt::get(C->get_word_ty(), number));

  BasicBlock *new_evaluator =
      BasicBlock::Create(C->context, "", C->current_function);

  C->builder.CreateCondBr(result, block, new_evaluator);

  evaluator = new_evaluator;
}
} // namespace blang
