#pragma once
#include "frontend/ast/AstIdentifierList.hh"
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstExtern : public AstNode {
  AstExtern();

  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;

  std::shared_ptr<AstIdentifierList> names;
};
} // namespace blang
