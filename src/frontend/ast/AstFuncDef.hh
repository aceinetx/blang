#pragma once
#include "frontend/ast/AstIdentifierList.hh"
#include "frontend/ast/AstNode.hh"
#include <ostream>

namespace blang {
struct AstFuncDef : public AstNode {
  AstFuncDef();

  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;
  void bindings(std::ostream &os) override;

  std::shared_ptr<AstNode> node_block;
  std::string name;
  std::shared_ptr<AstIdentifierList> args;
  std::vector<std::string> attrs;
};
} // namespace blang
