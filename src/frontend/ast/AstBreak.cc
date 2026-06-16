#include "frontend/ast/AstBreak.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstBreak::print(int indent) {
  printIndent(indent);
  fmt::print("- AstBreak\n");
}

fir::Value AstBreak::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  C->ir.br(C->while_statement_end_block);
  return {0};
}

} // namespace blang
