#include "frontend/ast/AstAssign.hh"
#include "Blang.hh"
#include <fmt/core.h>

namespace blang {
void AstAssign::print(int indent) {
  printIndent(indent);
  fmt::println("- AstAssign");

  dest->print(indent + 1);
  src->print(indent + 1);
}

llvm::Value *AstAssign::compile(Blang *blang, bool rvalue) {
  (void)rvalue;

  auto lv = blang->builder.CreateIntToPtr(dest->compile(blang, false),
                                          blang->builder.getPtrTy());
  auto rv = src->compile(blang, true);

  blang->builder.CreateStore(rv, lv);

  return rv;
}

} // namespace blang
