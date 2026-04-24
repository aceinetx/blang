#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstIndex : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang, bool rvalue) override;

  std::shared_ptr<AstNode> expression;
  std::shared_ptr<AstNode> index;
};
} // namespace blang
