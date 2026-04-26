#include "frontend/ast/AstFuncCall.hh"
#include "Assert.hh"
#include "Blang.hh"
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

llvm::Value *AstFuncCall::compile(Blang *blang, bool rvalue) {
  blangassert(rvalue);

  auto *callee_value = blang->builder.CreateIntToPtr(
      expression->compile(blang, false), blang->builder.getPtrTy());
  std::vector<llvm::Value *> arg_values = {};
  for (auto arg : args) {
    arg_values.push_back(arg->compile(blang, true));
  }

  auto *return_value = blang->builder.CreateCall(
      FunctionType::get(blang->get_word_ty(), {}, true), callee_value,
      arg_values);

  return return_value;
}
} // namespace blang
