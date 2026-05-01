#include "frontend/ast/AstIncDec.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstIncDec::print(int indent) {
  printIndent(indent);
  fmt::print("- AstIncDec {} {}\n", type, op);
  expression->print(indent + 1);
}

llvm::Value *AstIncDec::compile(CompilerContext *C, bool rvalue) {
  if (!rvalue) {
    throw LvalueException(location, "unary incdec");
  }

  auto value = C->builder.CreateIntToPtr(expression->compile(C, false),
                                         C->builder.getPtrTy());
  Value *output = nullptr;

  if (type == POST)
    output = C->builder.CreateLoad(C->get_word_ty(), value);

  if (op == INC)
    C->builder.CreateStore(
        C->builder.CreateAdd(C->builder.CreateLoad(C->get_word_ty(), value),
                             ConstantInt::get(C->get_word_ty(), 1)),
        value);
  else if (op == DEC)
    C->builder.CreateStore(
        C->builder.CreateSub(C->builder.CreateLoad(C->get_word_ty(), value),
                             ConstantInt::get(C->get_word_ty(), 1)),
        value);

  if (type == PRE)
    output = C->builder.CreateLoad(C->get_word_ty(), value);

  return output;
}

} // namespace blang
