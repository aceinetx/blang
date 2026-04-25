#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstFuncDef : public AstNode {
  AstFuncDef() = default;

  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang, bool rvalue) override;

  std::shared_ptr<AstNode> node_block;
  std::string name;
  std::vector<std::string> args;
};
} // namespace blang
