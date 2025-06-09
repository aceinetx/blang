#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstIncDec::print(int indent) const {
  printIndent(indent);
  std::cout << "AstIncDec" << "\n";
  printIndent(indent + 1);
  std::cout << "expr:" << "\n";
  expr->print(indent + 2);
}

AstIncDec::~AstIncDec() {
  delete expr;
}

bool AstIncDec::compile(Blang *blang) {
  if (!blang->expr_types.empty()) {
    if (blang->expr_types.top() == Blang::LVALUE) {
      blang->compile_error =
          "post/pre increment/decrement doesn't yet support lvalues";
      return false;
    }
    blang->expr_types.pop();
  }

  blang->expr_types.push(Blang::LVALUE);
  if (!expr->compile(blang))
    return false;

  Value *v = blang->values.top();
  blang->values.pop();

  if (type == POST)
    blang->values.push(blang->builder.CreateLoad(blang->getBWordTy(), v));

  if (op == INC)
    blang->builder.CreateStore(
        blang->builder.CreateAdd(
            blang->builder.CreateLoad(blang->getBWordTy(), v),
            ConstantInt::get(blang->getBWordTy(), 1)),
        v);
  else if (op == DEC)
    blang->builder.CreateStore(
        blang->builder.CreateSub(
            blang->builder.CreateLoad(blang->getBWordTy(), v),
            ConstantInt::get(blang->getBWordTy(), 1)),
        v);

  if (type == PRE)
    blang->values.push(blang->builder.CreateLoad(blang->getBWordTy(), v));

  return true;
}
