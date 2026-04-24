#include "frontend/ast/AstUnot.hh"
#include "Assert.hh"
#include "Blang.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void AstUnot::print(int indent) {
  printIndent(indent);
  fmt::println("- AstUnot");
  expression->print(indent + 1);
}

llvm::Value *AstUnot::compile(Blang *blang, bool rvalue) {
  blangassert(rvalue);

  Value *v = expression->compile(blang, true);

  Value *isZero = blang->builder.CreateICmpEQ(
      v, ConstantInt::get(blang->get_word_ty(), 0), "is_zero");

  Value *result = blang->builder.CreateSelect(
      isZero, ConstantInt::get(blang->get_word_ty(), 1),
      ConstantInt::get(blang->get_word_ty(), 0));

  return result;
}

} // namespace blang
