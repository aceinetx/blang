#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstStrRef::print(int indent) const {
  printIndent(indent);
  std::cout << "AstStrRef" << std::endl;
  printIndent(indent + 1);
  std::cout << "\"" << str << "\"\n";
}

bool AstStrRef::compile(Blang *blang) {
  blang->values.push(
      blang->builder.CreateGlobalStringPtr(str, "", 0U, &blang->fmodule));

  return true;
}
