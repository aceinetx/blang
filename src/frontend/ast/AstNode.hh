#pragma once
#include <llvm/IR/Value.h>
#include <location.hh>

namespace blang {
struct CompilerContext;

class AstNode {
public:
  AstNode() = default;
  AstNode(const AstNode &) = default;
  AstNode(AstNode &&) = delete;
  AstNode &operator=(const AstNode &) = default;
  AstNode &operator=(AstNode &&) = delete;
  virtual ~AstNode();

  virtual void print(int indent = 0);
  virtual llvm::Value *compile(CompilerContext *C, bool rvalue) = 0;
  virtual void bindings(std::ostream &os);

  class location location;

protected:
  void printIndent(int indent);
};
} // namespace blang
