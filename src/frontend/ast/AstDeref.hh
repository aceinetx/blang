#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstDeref : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang, bool rvalue = false) override;

  std::shared_ptr<AstNode> expression;
};
} // namespace blang
