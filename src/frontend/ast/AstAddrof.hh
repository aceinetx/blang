#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstAddrof : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang) override;

  std::shared_ptr<AstNode> expression;
};
} // namespace blang
