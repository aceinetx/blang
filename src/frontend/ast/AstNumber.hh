#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstNumber : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang, bool rvalue) override;

  bool is_rvalue() override;

  long number;
};
} // namespace blang
