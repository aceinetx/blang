#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstTernary : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;

  std::shared_ptr<AstNode> expression;
  std::shared_ptr<AstNode> true_expression;
  std::shared_ptr<AstNode> false_expression;
};
} // namespace blang
