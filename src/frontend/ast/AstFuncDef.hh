#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstFuncDef : public AstNode {
  AstFuncDef() = default;

  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang) override;

  std::vector<std::shared_ptr<AstNode>> statements;
  std::string name;
};
} // namespace blang
