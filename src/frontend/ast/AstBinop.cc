#include "AstBinop.hh"
#include "Assert.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstBinop::print(int indent) {
  printIndent(indent);
  fmt::println("- AstBinop {}", (int)op);

  left->print(indent + 1);
  right->print(indent + 1);
}

llvm::Value *AstBinop::compile(Blang *blang, bool rvalue) {
  blangassert(rvalue);
  auto lhs = left->compile(blang, true);
  auto rhs = right->compile(blang, true);
  llvm::Value *result = nullptr;

  switch (op) {
  case PLUS:
    result = blang->builder.CreateAdd(lhs, rhs);
    break;
  case MINUS:
    result = blang->builder.CreateSub(lhs, rhs);
    break;
  case MUL:
    result = blang->builder.CreateMul(lhs, rhs);
    break;
  case DIV:
    result = blang->builder.CreateSDiv(lhs, rhs);
    break;
  case EQUAL:
    result = blang->builder.CreateZExt(blang->builder.CreateICmpEQ(lhs, rhs),
                                       blang->get_word_ty());
    break;
  case NEQUAL:
    result = blang->builder.CreateZExt(blang->builder.CreateICmpNE(lhs, rhs),
                                       blang->get_word_ty());
    break;
  case GREATER:
    result = blang->builder.CreateZExt(blang->builder.CreateICmpSGT(lhs, rhs),
                                       blang->get_word_ty());
    break;
  case LESS:
    result = blang->builder.CreateZExt(blang->builder.CreateICmpSLT(lhs, rhs),
                                       blang->get_word_ty());
    break;
  case GREQ:
    result = blang->builder.CreateZExt(blang->builder.CreateICmpSGE(lhs, rhs),
                                       blang->get_word_ty());
    break;
  case LSEQ:
    result = blang->builder.CreateZExt(blang->builder.CreateICmpSLE(lhs, rhs),
                                       blang->get_word_ty());
    break;
  }

  return result;
}

} // namespace blang
