#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstCase : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;

  long number = 0;
};
} // namespace blang
