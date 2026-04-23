#include "frontend/ast/AstDerefRv.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstDerefRv::print(int indent) {
  printIndent(indent);
  fmt::println("- AstDerefRv");
  expression->print(indent + 1);
}

llvm::Value *AstDerefRv::compile(Blang *blang) {
  auto ptr_i64 = expression->compile(blang);
  auto ptr = blang->builder.CreateIntToPtr(ptr_i64, blang->builder.getPtrTy());
  auto value = blang->builder.CreateLoad(blang->get_word_ty(), ptr);

  return value;
}

} // namespace blang
