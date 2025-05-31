#include "../blang.hh"
#include "ast.hh"

using namespace blang;
using namespace llvm;

void AstBinaryOp::print(int indent) const {
  printIndent(indent);
  std::cout << "AstBinaryOp" << std::endl;
  printIndent(indent + 1);
  std::cout << "op: " << op << std::endl;
  printIndent(indent + 1);
  std::cout << "left:" << std::endl;
  left->print(indent + 2);
  printIndent(indent + 1);
  std::cout << "right:" << std::endl;
  right->print(indent + 2);
}

AstBinaryOp::~AstBinaryOp() {
  delete left;
  delete right;
}

bool AstBinaryOp::compile(Blang *blang) {
  if (!left->compile(blang))
    return false;
  if (!right->compile(blang))
    return false;

  Value *RHS = blang->values.top();
  blang->values.pop();
  Value *LHS = blang->values.top();
  blang->values.pop();

  Value *result = nullptr;

  if (op == "add") {
    result = blang->builder.CreateAdd(LHS, RHS);
  } else if (op == "sub") {
    result = blang->builder.CreateSub(LHS, RHS);
  } else if (op == "mul") {
    result = blang->builder.CreateMul(LHS, RHS);
  } else if (op == "div") {
    blang->compile_error = "div is a TODO";
    return false;
  }

  blang->values.push(result);

  return true;
}
