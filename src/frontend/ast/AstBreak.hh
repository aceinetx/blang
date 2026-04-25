#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstBreak : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang, bool rvalue) override;
};
} // namespace blang
