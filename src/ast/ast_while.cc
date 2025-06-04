#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstWhile::print(int indent) const {
  printIndent(indent);
  std::cout << "AstWhile" << "\n";
  printIndent(indent + 1);
  std::cout << "expr:" << "\n";
  expr->print(indent + 2);
}

AstWhile::~AstWhile() {
  delete expr;
  for (AstNode *n : body)
    delete n;
}

bool AstWhile::compile(Blang *blang) {
  unsigned int id = ++blang->whileID;

  blang->whiles.push({});
  While &w = blang->whiles.top();
  w.cmp = BasicBlock::Create(blang->context, fmt::format("wcmp{}", id),
                             blang->current_func);
  w.body = BasicBlock::Create(blang->context, fmt::format("wbody{}", id),
                              blang->current_func);
  w.end = BasicBlock::Create(blang->context, fmt::format("wend{}", id),
                             blang->current_func);

  blang->builder.CreateBr(w.cmp);
  blang->builder.SetInsertPoint(w.cmp);
  blang->expr_types.push(Blang::RVALUE);
  if (!expr->compile(blang))
    return false;

  Value *cmpValue = blang->values.top();
  blang->values.pop();
  cmpValue = blang->builder.CreateICmpNE(
      cmpValue, ConstantInt::get(blang->getBWordTy(), 0));

  blang->builder.CreateCondBr(cmpValue, w.body, w.end);

  blang->builder.SetInsertPoint(w.body);
  for (AstNode *n : body) {
    if (!n->compile(blang)) {
      return false;
    }
  }

  w.end->moveAfter(blang->builder.GetInsertBlock());

  if (!blang->builder.GetInsertBlock()->getTerminator()) {
    blang->builder.CreateBr(w.cmp);
  }

  blang->builder.SetInsertPoint(w.end);
  blang->whiles.pop();

  return true;
}
