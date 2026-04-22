#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstBinop : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang) override;

  std::shared_ptr<AstNode> left;
  std::shared_ptr<AstNode> right;

  enum { PLUS, MINUS, MUL, DIV } op;
};
} // namespace blang
