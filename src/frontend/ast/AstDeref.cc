#include "frontend/ast/AstDeref.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstDeref::print(int indent) {
  printIndent(indent);
  fmt::print("- AstDeref\n");
  expression->print(indent + 1);
}

llvm::Value *AstDeref::compile(CompilerContext *C, bool rvalue) {
  auto ptr_i64 = expression->compile(C, true);
  auto ptr = C->builder.CreateIntToPtr(ptr_i64, C->builder.getPtrTy());
  if (rvalue)
    ptr = C->builder.CreateLoad(C->get_word_ty(), ptr);

  return ptr;
}
} // namespace blang
