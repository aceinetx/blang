#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstDeref::print(int indent) const {
  printIndent(indent);
  std::cout << "AstDeref" << "\n";
  printIndent(indent + 1);
  std::cout << "expr:" << "\n";
  expr->print(indent + 2);
  std::cout << "times: " << times << "\n";
}

AstDeref::~AstDeref() {
  delete expr;
}

bool AstDeref::compile(Blang *blang) {
  blang->expr_types.push(Blang::RVALUE);
  if (!expr->compile(blang))
    return false;

  Value *value = blang->values.top();
  blang->values.pop();

  if (blang->expr_types.top() == Blang::LVALUE) {
    for (int i = 0; i < times - 1; i++) {
      value = blang->builder.CreateLoad(
          blang->builder.getInt64Ty(),
          blang->builder.CreateIntToPtr(value, blang->builder.getPtrTy()));
    }
    value = blang->builder.CreateIntToPtr(value, blang->builder.getPtrTy());
  }
  if (blang->expr_types.top() == Blang::RVALUE) {
    for (int i = 0; i < times; i++) {
      value = blang->builder.CreateLoad(
          blang->builder.getInt64Ty(),
          blang->builder.CreateIntToPtr(value, blang->builder.getPtrTy()));
    }
    value = blang->builder.CreateIntToPtr(value, blang->builder.getPtrTy());
  }

  /*
for (int i = 0; i < times + (blang->expr_types.top() == Blang::LVALUE); i++) {
value = blang->builder.CreateLoad(
  blang->builder.getInt64Ty(),
  blang->builder.CreateIntToPtr(value, blang->builder.getPtrTy()));
}

if (blang->expr_types.top() == Blang::LVALUE) {
value = blang->builder.CreateIntToPtr(value, blang->builder.getPtrTy());
}
  */

  blang->values.push(value);

  blang->expr_types.pop();

  return true;
}
