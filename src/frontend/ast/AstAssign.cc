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

llvm::Value *AstAssign::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;

  auto lv = C->builder.CreateIntToPtr(dest->compile(C, false),
                                          C->builder.getPtrTy());
  auto rv = src->compile(C, true);

  C->builder.CreateStore(rv, lv);

  return rv;
}

} // namespace blang
