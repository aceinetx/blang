#pragma once
#include "frontend/ast/AstNode.hh"
#include <memory>
#include <vector>

namespace blang {
struct AstBlock : public AstNode {
  void print(int indent = 0) override;
  fir::Value compile(CompilerContext *C, bool rvalue) override;

  std::vector<std::shared_ptr<AstNode>> children;
};
} // namespace blang
