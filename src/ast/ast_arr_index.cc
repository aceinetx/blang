#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstArrIndex::print(int indent) const {
  printIndent(indent);
  std::cout << "AstArrIndex" << std::endl;
  printIndent(indent + 1);
  std::cout << "expr:" << std::endl;
  expr->print(indent + 2);
  std::cout << "index:" << std::endl;
  index->print(indent + 2);
}

AstArrIndex::~AstArrIndex() {
  delete expr;
  delete index;
}

bool AstArrIndex::compile(Blang *blang) {
  blang->expr_types.push(Blang::RVALUE);
  if (!expr->compile(blang))
    return false;

  // Value *array =
  // blang->builder.CreateLoad(blang->builder.getPtrTy(), blang->values.top());
  Value *array = blang->builder.CreatePtrToInt(blang->values.top(),
                                               blang->builder.getPtrTy());
  blang->values.pop();

  blang->expr_types.push(Blang::RVALUE);
  if (!index->compile(blang))
    return false;

  Value *idx = blang->values.top();
  blang->values.pop();

  Value *element;
  if (blang->expr_types.top() == Blang::LVALUE) {
    element = blang->builder.CreateGEP(blang->getBWordTy(), array, idx);
  } else if (blang->expr_types.top() == Blang::RVALUE) {
    element = blang->builder.CreateLoad(
        blang->getBWordTy(),
        blang->builder.CreateGEP(blang->getBWordTy(), array, idx));
  }

  blang->values.push(element);

  blang->expr_types.pop();

  return true;
}
