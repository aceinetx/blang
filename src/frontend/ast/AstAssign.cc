#include "frontend/ast/AstAssign.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstAssign::print(int indent) {
  printIndent(indent);
  fmt::println("- AstAssign");

  lvalue->print(indent + 1);
  rvalue->print(indent + 1);
}

llvm::Value *AstAssign::compile(Blang *blang) {
  auto lv = lvalue->compile(blang);
  auto rv = rvalue->compile(blang);

  blang->builder.CreateStore(rv, lv);

  return rv;
}
} // namespace blang
