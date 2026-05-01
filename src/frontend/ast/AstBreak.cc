#include "frontend/ast/AstBreak.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstBreak::print(int indent) {
  printIndent(indent);
  fmt::print("- AstBreak\n");
}

llvm::Value *AstBreak::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  C->builder.CreateBr(C->while_statement_end_block);
  return nullptr;
}

} // namespace blang
