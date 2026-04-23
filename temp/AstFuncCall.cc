#include "frontend/ast/AstFuncCall.hh"
#include "Blang.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void AstFuncCall::print(int indent) {
  printIndent(indent);
  fmt::println("- AstFuncCall");
  expression->print(indent + 1);
  for (auto arg : args)
    arg->print(indent + 1);
}

llvm::Value *AstFuncCall::compile(Blang *blang) {
  auto *callee_value = blang->builder.CreateIntToPtr(expression->compile(blang),
                                                     blang->builder.getPtrTy());
  std::vector<llvm::Value *> arg_values = {};
  for (auto arg : args) {
    arg_values.push_back(arg->compile(blang));
  }

  auto *return_value = blang->builder.CreateCall(
      FunctionType::get(blang->get_word_ty(), {}, true), callee_value,
      arg_values);

  return return_value;
}
} // namespace blang
