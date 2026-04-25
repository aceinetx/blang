#include "frontend/ast/AstUinv.hh"
#include "Blang.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstUinv::print(int indent) {
  printIndent(indent);
  fmt::println("- AstUinv");
  expression->print(indent + 1);
}

llvm::Value *AstUinv::compile(Blang *blang, bool rvalue) {
  if (!rvalue) {
    throw LvalueException(location, "unary inverse");
  }

  auto value = expression->compile(blang, true);
  auto result = blang->builder.CreateSub(
      ConstantInt::get(blang->get_word_ty(), 0), value);

  return result;
}

} // namespace blang
