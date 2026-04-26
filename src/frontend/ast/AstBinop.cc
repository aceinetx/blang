#include "AstBinop.hh"
#include "Assert.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstBinop::print(int indent) {
  printIndent(indent);
  fmt::print("- AstBinop {}\n", (int)op);

  left->print(indent + 1);
  right->print(indent + 1);
}

llvm::Value *AstBinop::compile(CompilerContext *C, bool rvalue) {
  blangassert(rvalue);
  auto lhs = left->compile(C, true);
  auto rhs = right->compile(C, true);
  llvm::Value *result = nullptr;

  switch (op) {
  case PLUS:
    result = C->builder.CreateAdd(lhs, rhs);
    break;
  case MINUS:
    result = C->builder.CreateSub(lhs, rhs);
    break;
  case MUL:
    result = C->builder.CreateMul(lhs, rhs);
    break;
  case DIV:
    result = C->builder.CreateSDiv(lhs, rhs);
    break;
  case EQUAL:
    result = C->builder.CreateZExt(C->builder.CreateICmpEQ(lhs, rhs),
                                       C->get_word_ty());
    break;
  case NEQUAL:
    result = C->builder.CreateZExt(C->builder.CreateICmpNE(lhs, rhs),
                                       C->get_word_ty());
    break;
  case GREATER:
    result = C->builder.CreateZExt(C->builder.CreateICmpSGT(lhs, rhs),
                                       C->get_word_ty());
    break;
  case LESS:
    result = C->builder.CreateZExt(C->builder.CreateICmpSLT(lhs, rhs),
                                       C->get_word_ty());
    break;
  case GREQ:
    result = C->builder.CreateZExt(C->builder.CreateICmpSGE(lhs, rhs),
                                       C->get_word_ty());
    break;
  case LSEQ:
    result = C->builder.CreateZExt(C->builder.CreateICmpSLE(lhs, rhs),
                                       C->get_word_ty());
    break;
  case BITOR:
    result = C->builder.CreateOr(lhs, rhs);
    break;
  case BITAND:
    result = C->builder.CreateAnd(lhs, rhs);
    break;
  case BITSHL:
    result = C->builder.CreateShl(lhs, rhs);
    break;
  case BITSHR:
    result = C->builder.CreateLShr(lhs, rhs);
    break;
  case PERCENT:
    result = C->builder.CreateURem(lhs, rhs);
    break;
  }

  return result;
}

} // namespace blang
