#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstVarAssign::print(int indent) const {
  printIndent(indent);
  std::cout << "AstVarAssign" << std::endl;
  printIndent(indent + 1);
  std::cout << "name: " << name << std::endl;
  printIndent(indent + 1);
  std::cout << "expr:" << std::endl;
  expr->print(indent + 2);
}

AstVarAssign::~AstVarAssign() {
  delete expr;
}

bool AstVarAssign::compile(Blang *blang) {
  Variable *var = blang->getVariable(name);
  if (!var) {
    blang->compile_error = fmt::format("undefined variable: {}", name);
    return false;
  }

  if (!expr->compile(blang))
    return false;

  Value *expr_value = blang->values.top();
  blang->values.pop();

  blang->builder.CreateStore(expr_value, var->value);

  blang->values.push(expr_value);

  return true;
}
