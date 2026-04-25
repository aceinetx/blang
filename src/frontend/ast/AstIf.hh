#pragma once
#include "frontend/ast/AstBlock.hh"
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstIf : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang, bool rvalue) override;

  std::shared_ptr<AstBlock> body;
  std::shared_ptr<AstNode> expression;
};
} // namespace blang
