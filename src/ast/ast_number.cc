#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstNumber::print(int indent) const {
  printIndent(indent);
  std::cout << "AstNumber" << std::endl;
  printIndent(indent + 1);
  std::cout << "value: " << value << std::endl;
}

bool AstNumber::compile(Blang *blang) {
  Value *v = ConstantInt::get(blang->builder.getInt64Ty(), value);
  blang->values.push(v);
  return true;
}
