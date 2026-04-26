#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstSwitch : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;

  void add_case(CompilerContext *C, long number, llvm::BasicBlock *block);

  std::shared_ptr<AstNode> expression;
  std::shared_ptr<AstNode> statement;

  llvm::BasicBlock *end;
  llvm::BasicBlock *body;
  llvm::BasicBlock *evaluator;
  llvm::Value *value;
};
} // namespace blang
