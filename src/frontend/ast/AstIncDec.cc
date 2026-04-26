#include "frontend/ast/AstIncDec.hh"
#include "Blang.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstIncDec::print(int indent) {
  printIndent(indent);
  fmt::println("- AstIncDec {} {}", type, op);
  expression->print(indent + 1);
}

llvm::Value *AstIncDec::compile(Blang *blang, bool rvalue) {
  if (!rvalue) {
    throw LvalueException(location, "unary incdec");
  }

  auto value = blang->builder.CreateIntToPtr(expression->compile(blang, false),
                                             blang->builder.getPtrTy());
  Value *output = nullptr;

  if (type == POST)
    output = blang->builder.CreateLoad(blang->get_word_ty(), value);

  if (op == INC)
    blang->builder.CreateStore(
        blang->builder.CreateAdd(
            blang->builder.CreateLoad(blang->get_word_ty(), value),
            ConstantInt::get(blang->get_word_ty(), 1)),
        value);
  else if (op == DEC)
    blang->builder.CreateStore(
        blang->builder.CreateSub(
            blang->builder.CreateLoad(blang->get_word_ty(), value),
            ConstantInt::get(blang->get_word_ty(), 1)),
        value);

  if (type == PRE)
    output = blang->builder.CreateLoad(blang->get_word_ty(), value);

  return output;
}

} // namespace blang
