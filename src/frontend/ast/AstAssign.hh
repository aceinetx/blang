#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstAssign : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang, bool rvalue) override;

  std::shared_ptr<AstNode> dest;
  std::shared_ptr<AstNode> src;
};
} // namespace blang
