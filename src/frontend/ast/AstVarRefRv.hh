#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstVarRefRv : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang) override;

  std::string name = "";
};
} // namespace blang
