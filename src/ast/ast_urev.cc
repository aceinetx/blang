#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstUrev::print(int indent) const {
  printIndent(indent);
  std::cout << "AstUrev" << "\n";
  printIndent(indent + 1);
}

AstUrev::~AstUrev() {
  delete value;
}

bool AstUrev::compile(Blang *blang) {
  if (blang->expr_types.top() == Blang::LVALUE) {
    blang->compile_error =
        fmt::format("unary minus can only be applied to rvalues");
    return false;
  }

  blang->expr_types.push(Blang::RVALUE);
  if (!value->compile(blang))
    return false;

  Value *v = blang->values.top();
  blang->values.pop();

  Value *result =
      blang->builder.CreateSub(ConstantInt::get(blang->getBWordTy(), 0), v);
  blang->values.push(result);

  blang->expr_types.pop();

  return true;
}
