#include "frontend/ast/AstBreak.hh"
#include "Blang.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void AstBreak::print(int indent) {
  printIndent(indent);
  fmt::println("- AstBreak");
}

llvm::Value *AstBreak::compile(Blang *blang, bool rvalue) {
  (void)rvalue;
  blang->builder.CreateBr(blang->while_statement_end_block);
  return nullptr;
}

} // namespace blang
