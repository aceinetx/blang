#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstFuncDef : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile() override;

  std::vector<std::shared_ptr<AstNode>> statements;
};
} // namespace blang
