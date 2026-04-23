#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstRoot : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang, bool rvalue = true) override;

  bool is_rvalue() override;

  std::vector<std::shared_ptr<AstNode>> children;
};
} // namespace blang
