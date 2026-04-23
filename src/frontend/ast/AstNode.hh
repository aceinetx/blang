#pragma once
#include <llvm/IR/Value.h>

namespace blang {
struct Blang;

class AstNode {
public:
  AstNode() = default;
  AstNode(const AstNode &) = default;
  AstNode(AstNode &&) = delete;
  AstNode &operator=(const AstNode &) = default;
  AstNode &operator=(AstNode &&) = delete;
  virtual ~AstNode();

  virtual void print(int indent = 0);
  virtual llvm::Value *compile(Blang *blang, bool rvalue) = 0;

  virtual bool is_rvalue() = 0;

protected:
  void printIndent(int indent);
};
} // namespace blang
