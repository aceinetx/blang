#include "frontend/ast/AstDerefLv.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstDerefLv::print(int indent) {
  printIndent(indent);
  fmt::println("- AstDerefLv");
  expression->print(indent + 1);
}

llvm::Value *AstDerefLv::compile(Blang *blang) {
  auto ptr_i64 = expression->compile(blang);
  auto ptr = blang->builder.CreateIntToPtr(ptr_i64, blang->builder.getPtrTy());

  return ptr;
}

} // namespace blang
