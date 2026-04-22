#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstReturn : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile() override;

  std::shared_ptr<AstNode> expression;
};
} // namespace blang
