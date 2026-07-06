#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstUinv : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;

  std::shared_ptr<AstNode> expression;
};
} // namespace blang
