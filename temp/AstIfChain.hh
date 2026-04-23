#pragma once
#include "frontend/ast/AstElse.hh"
#include "frontend/ast/AstElseIf.hh"
#include "frontend/ast/AstIf.hh"
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstIfChain : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(Blang *blang) override;

  std::shared_ptr<AstIf> begin_if;
  std::vector<std::shared_ptr<AstElseIf>> elifs;
  std::shared_ptr<AstElse> end_else;
};
} // namespace blang
