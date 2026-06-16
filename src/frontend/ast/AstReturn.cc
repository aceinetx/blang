#include "frontend/ast/AstReturn.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstReturn::print(int indent) {
  printIndent(indent);
  fmt::print("- AstReturn\n");
  expression->print(indent + 1);
}

fir::Value AstReturn::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  auto value = expression->compile(C, true);
  C->ir.ret(value);
  return {0};
}
} // namespace blang
