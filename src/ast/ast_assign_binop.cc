#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstAssignBinop::print(int indent) const {
  printIndent(indent);
  std::cout << "AstAssignBinop" << "\n";
  printIndent(indent + 1);
}

AstAssignBinop::~AstAssignBinop() {
  delete var;
  delete value;
}

bool AstAssignBinop::compile(Blang *blang) {
  Value *dest, *LHS, *RHS, *result;

  result = nullptr;

  blang->expr_types.push(Blang::LVALUE);
  if (!var->compile(blang))
    return false;
  dest = blang->values.top();
  blang->values.pop();

  blang->expr_types.push(Blang::RVALUE);
  if (!value->compile(blang))
    return false;
  RHS = blang->values.top();
  blang->values.pop();

  blang->expr_types.push(Blang::RVALUE);
  if (!var->compile(blang))
    return false;
  LHS = blang->values.top();
  blang->values.pop();

  if (op == "add") {
    result = blang->builder.CreateAdd(LHS, RHS);
  } else if (op == "sub") {
    result = blang->builder.CreateSub(LHS, RHS);
  } else if (op == "mul") {
    result = blang->builder.CreateMul(LHS, RHS);
  } else if (op == "div") {
    blang->compile_error = "div is a TODO";
    return false;
  } else if (op == "bitshl") {
    result = blang->builder.CreateShl(LHS, RHS);
  } else if (op == "bitshr") {
    result = blang->builder.CreateLShr(LHS, RHS);
  } else if (op == "bitand") {
    result = blang->builder.CreateAnd(LHS, RHS);
  } else if (op == "bitor") {
    result = blang->builder.CreateOr(LHS, RHS);
  }

  blang->builder.CreateStore(result, dest, blang->getBWordTy());

  return true;
}
