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
  If &i = blang->ifs.top();

  BasicBlock *part = BasicBlock::Create(
      blang->context, fmt::format("part{}of{}", i.parts.size(), blang->ifID),
      blang->current_func, i.end);

  BasicBlock *fallthrough = BasicBlock::Create(
      blang->context,
      fmt::format("fallthough{}of{}", i.parts.size(), blang->ifID),
      blang->current_func);
  fallthrough->moveAfter(i.fallthrough);

  blang->builder.SetInsertPoint(i.fallthrough);
  blang->expr_types.push(Blang::RVALUE);
  if (!expr->compile(blang))
    return false;

  Value *cmpValue = blang->values.top();
  blang->values.pop();
  cmpValue = blang->builder.CreateTrunc(cmpValue, blang->builder.getInt1Ty());

  blang->builder.CreateCondBr(cmpValue, part, fallthrough);

  i.fallthrough = fallthrough;

  blang->builder.SetInsertPoint(part);
  for (AstNode *n : body) {
    if (!n->compile(blang)) {
      return false;
    }
  }

  blang->builder.CreateBr(i.end);

  return true;
}
