#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstBinop : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;

  std::shared_ptr<AstNode> left;
  std::shared_ptr<AstNode> right;

  enum {
    PLUS,
    MINUS,
    MUL,
    DIV,
    EQUAL,
    NEQUAL,
    GREATER,
    LESS,
    GREQ,
    LSEQ,
    BITOR,
    BITAND,
    BITSHL,
    BITSHR,
    PERCENT,
  } op;
};
} // namespace blang
