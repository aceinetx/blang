#pragma once
#include "frontend/ast/AstNode.hh"
#include <memory>

namespace blang {
struct AstDeref : public AstNode {
  void print(int indent = 0) override;
  fir::Value compile(CompilerContext *C, bool rvalue = false) override;

  std::shared_ptr<AstNode> expression;
};
} // namespace blang
