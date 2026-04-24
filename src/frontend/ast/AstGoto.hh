#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstGoto : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang, bool rvalue) override;

  std::string name = "";
};
} // namespace blang
