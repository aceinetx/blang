#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstVarAssign::print(int indent) const {
  printIndent(indent);
  std::cout << "AstVarAssign" << std::endl;
  printIndent(indent + 1);
  std::cout << "lexpr:" << std::endl;
  lexpr->print(indent + 2);
  printIndent(indent + 1);
  std::cout << "rexpr:" << std::endl;
  rexpr->print(indent + 2);
}

AstVarAssign::~AstVarAssign() {
  delete lexpr;
  delete rexpr;
}

bool AstVarAssign::compile(Blang *blang) {
  blang->expr_types.push(Blang::LVALUE);
  if (!lexpr->compile(blang))
    return false;
  Value *var = blang->values.top();
  blang->values.pop();

  blang->expr_types.push(Blang::RVALUE);
  if (!rexpr->compile(blang))
    return false;

  Value *expr_value = blang->values.top();
  blang->values.pop();

  blang->builder.CreateStore(expr_value, var);

  blang->values.push(expr_value);

  return true;
}
