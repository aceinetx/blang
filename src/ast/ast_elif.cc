#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstElif::print(int indent) const {
  printIndent(indent);
  std::cout << "AstElif" << "\n";
  printIndent(indent + 1);
  std::cout << "expr:" << "\n";
  expr->print(indent + 2);
  body->print(indent + 2);
}

AstElif::~AstElif() {
  delete expr;
  delete body;
}

bool AstElif::compile(Blang *blang) {
  blang->expr_types.push(Blang::RVALUE);
  if (!expr->compile(blang))
    return false;

  If &parentIf = blang->ifs.top();

  Value *cmpValue = blang->values.top();
  blang->values.pop();

  unsigned int id = ++blang->ifID;
  BasicBlock *trueBlock = BasicBlock::Create(
      blang->context, fmt::format("{}t", id), blang->current_func);
  BasicBlock *falseBlock = BasicBlock::Create(
      blang->context, fmt::format("{}f", id), blang->current_func);

  blang->builder.SetInsertPoint(parentIf.falseBlock);
  blang->builder.CreateCondBr(cmpValue, trueBlock, falseBlock);

  blang->builder.SetInsertPoint(trueBlock);
  if (!body->compile(blang))
    return false;

  if (!blang->builder.GetInsertBlock()->getTerminator())
    blang->builder.CreateBr(parentIf.mergeBlock);

  parentIf.falseBlock = falseBlock;
  parentIf.elif = true;

  blang->builder.SetInsertPoint(falseBlock);

  return true;
}
