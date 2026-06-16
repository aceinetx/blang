#pragma once
#include "frontend/ast/AstNode.hh"
#include <memory>
#include <vector>

namespace blang {
struct AstFuncCall : public AstNode {
  void print(int indent = 0) override;
  fir::Value compile(CompilerContext *C, bool rvalue) override;

  std::shared_ptr<AstNode> expression;
  std::vector<std::shared_ptr<AstNode>> args;
};
} // namespace blang
