#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstElseIf : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang, bool rvalue) override;

  std::vector<std::shared_ptr<AstNode>> body;
  std::shared_ptr<AstNode> expression;
};
} // namespace blang
