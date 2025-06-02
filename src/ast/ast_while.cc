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
  BasicBlock *cmp = BasicBlock::Create(
      blang->context, fmt::format("wcmp{}", id), blang->current_func);
  BasicBlock *body_block = BasicBlock::Create(
      blang->context, fmt::format("wbody{}", id), blang->current_func);
  BasicBlock *end = BasicBlock::Create(
      blang->context, fmt::format("wend{}", id), blang->current_func);

  blang->builder.CreateBr(cmp);
  blang->builder.SetInsertPoint(cmp);
  blang->expr_types.push(Blang::RVALUE);
  if (!expr->compile(blang))
    return false;

  Value *cmpValue = blang->values.top();
  blang->values.pop();
  cmpValue = blang->builder.CreateTrunc(cmpValue, blang->builder.getInt1Ty());

  blang->builder.CreateCondBr(cmpValue, body_block, end);

  blang->builder.SetInsertPoint(body_block);
  for (AstNode *n : body) {
    if (!n->compile(blang)) {
      return false;
    }
  }

  if (!body_block->getTerminator()) {
    blang->builder.SetInsertPoint(body_block);
    blang->builder.CreateBr(cmp);
  }

  blang->builder.SetInsertPoint(end);

  return true;
}
