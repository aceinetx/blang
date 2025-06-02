#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstAddrof::print(int indent) const {
  printIndent(indent);
  std::cout << "AstAddrof" << "\n";
  printIndent(indent + 1);
  std::cout << "expr:" << "\n";
  expr->print(indent + 2);
  std::cout << "times: " << times << "\n";
}

AstAddrof::~AstAddrof() {
  delete expr;
}

bool AstAddrof::compile(Blang *blang) {
  blang->expr_types.push(Blang::LVALUE);
  if (!expr->compile(blang))
    return false;

  Value *value = blang->values.top();
  blang->values.pop();

  blang->values.push(blang->builder.CreatePtrToInt(value, blang->getBWordTy()));

  blang->expr_types.pop();

  return true;
}
