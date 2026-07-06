#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstIf : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;

  std::shared_ptr<AstNode> expression;
  std::shared_ptr<AstNode> then_node;
  std::shared_ptr<AstNode> else_node;
};
} // namespace blang
