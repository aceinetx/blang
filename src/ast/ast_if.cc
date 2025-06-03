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
  unsigned int id = ++blang->ifID;
  BasicBlock *true_block = BasicBlock::Create(
      blang->context, std::format("{}t", id), blang->current_func);
  BasicBlock *false_block = BasicBlock::Create(
      blang->context, std::format("{}f", id), blang->current_func);
  BasicBlock *merge_block = BasicBlock::Create(
      blang->context, std::format("{}m", id), blang->current_func);

  blang->expr_types.push(Blang::RVALUE);
  if (!expr->compile(blang))
    return false;
  Value *cmpValue = blang->values.top();
  blang->values.pop();
  cmpValue = blang->builder.CreateICmpNE(
      cmpValue, ConstantInt::get(blang->getBWordTy(), 0));

  blang->builder.CreateCondBr(cmpValue, true_block, false_block);

  blang->ifs.push({});
  If &i = blang->ifs.top();
  i.true_block = true_block;
  i.false_block = false_block;
  i.merge_block = merge_block;

  blang->builder.SetInsertPoint(true_block);
  for (AstNode *n : body) {
    if (!n->compile(blang)) {
      return false;
    }
  }

  if (!blang->builder.GetInsertBlock()->getTerminator())
    blang->builder.CreateBr(merge_block);

  blang->builder.SetInsertPoint(false_block);

  return true;
}
