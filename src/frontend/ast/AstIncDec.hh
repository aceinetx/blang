#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstIncDec : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;

  std::shared_ptr<AstNode> expression;

  enum {
    POST,
    PRE,
  };

  enum {
    INC,
    DEC,
  };

  int type, op;
};
} // namespace blang
