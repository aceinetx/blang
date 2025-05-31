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

  blang->values.push(
      blang->builder.CreateLoad(blang->builder.getInt64Ty(), var->value));
  return true;
}
