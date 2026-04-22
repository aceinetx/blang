#include "AstBinop.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstBinop::print(int indent) {
  printIndent(indent);
  fmt::println("- AstBinop {}", (int)op);

  left->print(indent + 1);
  right->print(indent + 1);
}

llvm::Value *AstBinop::compile(Blang *blang) {
  assert(name != "");

  auto lhs = left->compile(blang);
  auto rhs = right->compile(blang);
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
  }

  return result;
}

} // namespace blang
