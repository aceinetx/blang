#include "frontend/ast/AstUinv.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstUinv::print(int indent) {
  printIndent(indent);
  fmt::print("- AstUinv\n");
  expression->print(indent + 1);
}

llvm::Value *AstUinv::compile(CompilerContext *C, bool rvalue) {
  if (!rvalue) {
    throw LvalueException(location, "unary inverse");
  }

  auto value = expression->compile(C, true);
  auto result =
      C->builder.CreateSub(ConstantInt::get(C->get_word_ty(), 0), value);

  return result;
}

} // namespace blang
