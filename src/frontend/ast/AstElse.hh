#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstElse : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang) override;

  std::vector<std::shared_ptr<AstNode>> body;
};
} // namespace blang
