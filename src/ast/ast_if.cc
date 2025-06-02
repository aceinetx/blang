#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstIf::print(int indent) const {
  printIndent(indent);
  std::cout << "AstIf" << "\n";
  printIndent(indent + 1);
  std::cout << "expr:" << "\n";
  expr->print(indent + 2);
}

AstIf::~AstIf() {
  delete expr;
  for (AstNode *n : body)
    delete n;
}

bool AstIf::compile(Blang *blang) {
  blang->expr_types.push(Blang::RVALUE);
  if (!expr->compile(blang))
    return false;

  Value *cmpValue = blang->values.top();
  blang->values.pop();
  cmpValue = blang->builder.CreateTrunc(cmpValue, blang->builder.getInt1Ty());

  int id = ++blang->ifID;
  BasicBlock *trueBlock = BasicBlock::Create(
      blang->context, fmt::format("{}t", id), blang->current_func);
  BasicBlock *falseBlock = BasicBlock::Create(
      blang->context, fmt::format("{}f", id), blang->current_func);
  BasicBlock *mergeBlock = BasicBlock::Create(
      blang->context, fmt::format("{}m", id), blang->current_func);

  blang->builder.CreateCondBr(cmpValue, trueBlock, falseBlock);

  blang->ifs.push({});
  If &i = blang->ifs.top();
  i.trueBlock = trueBlock;
  i.falseBlock = falseBlock;
  i.mergeBlock = mergeBlock;
  i.elif = false;

  blang->builder.SetInsertPoint(trueBlock);

  for (AstNode *n : body) {
    if (!n->compile(blang))
      return false;
  }

  if (!blang->builder.GetInsertBlock()->getTerminator())
    blang->builder.CreateBr(mergeBlock);

  blang->builder.SetInsertPoint(falseBlock);

  return true;
}
