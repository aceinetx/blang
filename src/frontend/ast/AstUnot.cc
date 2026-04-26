#include "frontend/ast/AstUnot.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstUnot::print(int indent) {
  printIndent(indent);
  fmt::print("- AstUnot\n");
  expression->print(indent + 1);
}

llvm::Value *AstUnot::compile(CompilerContext *C, bool rvalue) {
  if (!rvalue)
    throw LvalueException(location, "unary not");

  Value *v = expression->compile(C, true);

  Value *isZero = C->builder.CreateICmpEQ(
      v, ConstantInt::get(C->get_word_ty(), 0), "is_zero");

  Value *result = C->builder.CreateSelect(
      isZero, ConstantInt::get(C->get_word_ty(), 1),
      ConstantInt::get(C->get_word_ty(), 0));

  return result;
}

} // namespace blang
