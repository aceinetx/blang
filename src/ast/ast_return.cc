#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;

void AstReturn::print(int indent) const {
  printIndent(indent);
  std::cout << "AstReturn" << std::endl;
  if (expr) {
    expr->print(indent + 1);
  }
}

AstReturn::~AstReturn() {
  delete expr;
}

bool AstReturn::compile(Blang *blang) {
  if (!expr->compile(blang))
    return false;

  blang->builder.CreateRet(blang->values.top());
  blang->values.pop();

  return true;
}
