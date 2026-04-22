#include "frontend/ast/AstAssign.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstAssign::print(int indent) {
  printIndent(indent);
  fmt::println("- AstAssign");

  lvalue->print(indent + 1);

  printIndent(indent);
  fmt::println("=");

  rvalue->print(indent + 2);
}

llvm::Value *AstAssign::compile(Blang *blang) {
  assert(name != "");

  auto lv = lvalue->compile(blang);
  auto rv = rvalue->compile(blang);

  blang->builder.CreateStore(rv, lv);

  return rv;
}
} // namespace blang
