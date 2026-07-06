#pragma once
#include "frontend/ast/AstAutoVarList.hh"
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstAutoVar : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;

  std::shared_ptr<AstAutoVarList> list;
};
} // namespace blang
