#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstUnot::print(int indent) const {
  printIndent(indent);
  std::cout << "AstUnot" << "\n";
  printIndent(indent + 1);
}

AstUnot::~AstUnot() {
  delete value;
}

bool AstUnot::compile(Blang *blang) {
  Value *result, *v;

  if (blang->expr_types.top() == Blang::LVALUE) {
    blang->compile_error =
        fmt::format("unary not can only be applied to rvalues");
    return false;
  }
  blang->expr_types.pop();

  blang->expr_types.push(Blang::RVALUE);
  if (!value->compile(blang))
    return false;

  v = blang->values.top();
  blang->values.pop();

  Value *isZero = blang->builder.CreateICmpEQ(
      v, ConstantInt::get(blang->builder.getInt32Ty(), 0), "is_zero");

  result = blang->builder.CreateSelect(
      isZero, ConstantInt::get(blang->getBWordTy(), 1),
      ConstantInt::get(blang->getBWordTy(), 0));

  blang->values.push(result);

  return true;
}
