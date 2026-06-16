#include "frontend/ast/AstCase.hh"
#include "Assert.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstCase::print(int indent) {
  printIndent(indent);
  fmt::print("- AstCase {}\n", number);
}

fir::Value AstCase::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  blangassert(C->last_switch);

  auto block = C->ir.block();
  C->ir.br(block);

  C->last_switch->add_case(C, number, block);

  C->ir.set_insert_point(block);
  return {0};
}

} // namespace blang
