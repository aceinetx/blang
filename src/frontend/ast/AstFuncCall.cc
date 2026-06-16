#include "frontend/ast/AstFuncCall.hh"
#include "Assert.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstFuncCall::print(int indent) {
  printIndent(indent);
  fmt::print("- AstFuncCall\n");
  expression->print(indent + 1);
  for (auto arg : args)
    arg->print(indent + 1);
}

fir::Value AstFuncCall::compile(CompilerContext *C, bool rvalue) {
  blangassert(rvalue);
  blangassert(0 && "Unsupported");

#if 0
  auto callee_value = expression->compile(C, true);
  callee_value = frIR_cast(&C->ir, &callee_value, C->get_word_ptr_ty());
  std::array<fir::Value*, 8> arg_values = {};
  for (auto arg : args) {
    arg_values.push_back(arg->compile(C, true));
  }

  auto* return_value = frIR_cr
#endif

  return {0};
}
} // namespace blang
