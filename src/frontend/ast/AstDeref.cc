#include "frontend/ast/AstDeref.hh"
#include "Blang.hh"
#include <fmt/core.h>

namespace blang {
void AstDeref::print(int indent) {
  printIndent(indent);
  fmt::print("- AstDeref\n");
  expression->print(indent + 1);
}

llvm::Value *AstDeref::compile(Blang *blang, bool rvalue) {
  auto ptr_i64 = expression->compile(blang, true);
  auto ptr = blang->builder.CreateIntToPtr(ptr_i64, blang->builder.getPtrTy());
  if (rvalue)
    ptr = blang->builder.CreateLoad(blang->get_word_ty(), ptr);

  return ptr;
}
} // namespace blang
