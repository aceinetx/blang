#pragma once
#include <llvm/IR/Value.h>

namespace blang {
class AstNode {
public:
  AstNode() = default;
  AstNode(const AstNode &) = default;
  AstNode(AstNode &&) = delete;
  AstNode &operator=(const AstNode &) = default;
  AstNode &operator=(AstNode &&) = delete;
  virtual ~AstNode();

  virtual void print(int indent = 0);
  virtual llvm::Value *compile() = 0;

protected:
  void printIndent(int indent);
};
} // namespace blang
