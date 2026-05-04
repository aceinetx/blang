#include "frontend/ast/AstFuncCall.hh"
#include "Assert.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstFuncCall::print(int indent) {
  printIndent(indent);
  fmt::print("- AstFuncCall\n");
  expression->print(indent + 1);
  for (auto arg : args)
    arg->print(indent + 1);
}

llvm::Value *AstFuncCall::compile(CompilerContext *C, bool rvalue) {
  blangassert(rvalue);

  auto *callee_value = C->builder.CreateIntToPtr(expression->compile(C, true),
                                                 C->builder.getPtrTy());
  std::vector<llvm::Value *> arg_values = {};
  for (auto arg : args) {
    arg_values.push_back(arg->compile(C, true));
  }

  auto *return_value = C->builder.CreateCall(
      FunctionType::get(C->get_word_ty(), {}, true), callee_value, arg_values);

  return return_value;
}
} // namespace blang
