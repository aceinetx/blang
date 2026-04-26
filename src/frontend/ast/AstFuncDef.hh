#pragma once
#include "frontend/ast/AstIdentifierList.hh"
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstFuncDef : public AstNode {
  AstFuncDef();

  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang, bool rvalue) override;

  std::shared_ptr<AstNode> node_block;
  std::string name;
  std::shared_ptr<AstIdentifierList> args;
  std::vector<std::string> attrs;
};
} // namespace blang
