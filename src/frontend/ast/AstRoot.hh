#pragma once
#include "frontend/ast/AstNode.hh"
#include <ostream>

namespace blang {
struct AstRoot : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue = true) override;
  void bindings(std::ostream &os) override;

  std::vector<std::shared_ptr<AstNode>> children;
};
} // namespace blang
