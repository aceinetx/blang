#include "frontend/ast/AstDeref.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstDeref::print(int indent) {
  printIndent(indent);
  fmt::print("- AstDeref\n");
  expression->print(indent + 1);
}

fir::Value AstDeref::compile(CompilerContext *C, bool rvalue) {
  auto ptr_i64 = expression->compile(C, true);
  auto ptr = C->ir.cast(ptr_i64, C->get_word_ptr_ty());
  if (rvalue)
    ptr = C->ir.load(ptr, C->get_word_ty());

  return ptr;
}
} // namespace blang
