#include "frontend/ast/AstAssign.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstAssign::print(int indent) {
  printIndent(indent);
  fmt::print("- AstAssign\n");

  dest->print(indent + 1);
  src->print(indent + 1);
}

fir::Value AstAssign::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;

  auto lv = dest->compile(C, false);
  auto lv_ptr = C->ir.cast(lv, C->get_word_ptr_ty());

  auto rv = src->compile(C, true);

  C->ir.store(lv_ptr, rv);

  return rv;
}

} // namespace blang
