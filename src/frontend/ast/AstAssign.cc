#include "frontend/ast/AstAssign.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstAssign::print(int indent) {
  printIndent(indent);
  fmt::println("- AstAssign");

  dest->print(indent + 1);
  src->print(indent + 1);
}

llvm::Value *AstAssign::compile(Blang *blang, bool rvalue) {
  (void)rvalue;

  if (dest->is_rvalue()) {
    throw std::runtime_error("expected lvalue in lhs assignment");
  }
  if (!src->is_rvalue()) {
    throw std::runtime_error("expected rvalue in rhs assignment");
  }
  auto lv = dest->compile(blang, false);
  auto rv = src->compile(blang, true);

  blang->builder.CreateStore(rv, lv);

  return rv;
}

bool AstAssign::is_rvalue() {
  return true;
}

} // namespace blang
