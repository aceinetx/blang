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

  result =
      blang->builder.CreateXor(v, ConstantInt::get(blang->getBWordTy(), 1));

  blang->values.push(result);

  return true;
}
