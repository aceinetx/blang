#pragma once
#include "frontend/ast/AstNode.hh"
#include <memory>

namespace blang {
struct AstUinv : public AstNode {
  void print(int indent = 0) override;
  fir::Value compile(CompilerContext *C, bool rvalue) override;

  std::shared_ptr<AstNode> expression;
};
} // namespace blang
