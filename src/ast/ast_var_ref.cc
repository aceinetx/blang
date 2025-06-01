#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstVarRef::print(int indent) const {
  printIndent(indent);
  std::cout << name << std::endl;
}

bool AstVarRef::compile(Blang *blang) {
  Variable *var = blang->getVariable(name);
  if (!var) {
    blang->compile_error = fmt::format("undefined variable: {}", name);
    return false;
  }

  if (blang->expr_types.top() == Blang::RVALUE) {
    blang->values.push(
        blang->builder.CreateLoad(blang->getBWordTy(), var->value));
  } else if (blang->expr_types.top() == Blang::LVALUE) {
    blang->values.push(var->value);
  }

  blang->expr_types.pop();

  return true;
}
