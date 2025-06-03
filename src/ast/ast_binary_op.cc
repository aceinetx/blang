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
  // TODO: BinaryOp in rvalues

  blang->expr_types.push(Blang::RVALUE);
  if (!left->compile(blang))
    return false;
  blang->expr_types.push(Blang::RVALUE);
  if (!right->compile(blang))
    return false;

  Value *RHS = blang->values.top();
  blang->values.pop();
  Value *LHS = blang->values.top();
  blang->values.pop();

  // TODO: Implement a check if a valid lvalue binary operation is here

  Value *result = nullptr;

  if (op == "add") {
    result = blang->builder.CreateAdd(LHS, RHS);
  } else if (op == "sub") {
    result = blang->builder.CreateSub(LHS, RHS);
  } else if (op == "mul") {
    result = blang->builder.CreateMul(LHS, RHS);
  } else if (op == "div") {
    result = blang->builder.CreateSDiv(LHS, RHS);
  } else if (op == "equal") {
    result = blang->builder.CreateZExt(blang->builder.CreateICmpEQ(LHS, RHS),
                                       blang->getBWordTy());
  } else if (op == "nequal") {
    result = blang->builder.CreateZExt(blang->builder.CreateICmpNE(LHS, RHS),
                                       blang->getBWordTy());
  } else if (op == "greater") {
    result = blang->builder.CreateZExt(blang->builder.CreateICmpSGT(LHS, RHS),
                                       blang->getBWordTy());
  } else if (op == "less") {
    result = blang->builder.CreateZExt(blang->builder.CreateICmpSLT(LHS, RHS),
                                       blang->getBWordTy());
  } else if (op == "greq") {
    result = blang->builder.CreateZExt(blang->builder.CreateICmpSGE(LHS, RHS),
                                       blang->getBWordTy());
  } else if (op == "lseq") {
    result = blang->builder.CreateZExt(blang->builder.CreateICmpSLE(LHS, RHS),
                                       blang->getBWordTy());
  } else if (op == "bitshl") {
    result = blang->builder.CreateShl(LHS, RHS);
  } else if (op == "bitshr") {
    result = blang->builder.CreateLShr(LHS, RHS);
  } else if (op == "bitand") {
    result = blang->builder.CreateAnd(LHS, RHS);
  } else if (op == "bitor") {
    result = blang->builder.CreateOr(LHS, RHS);
  }
  blang->values.push(result);

  blang->expr_types.pop();

  return true;
}
