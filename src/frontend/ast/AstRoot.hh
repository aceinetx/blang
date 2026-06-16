#pragma once
#include "frontend/ast/AstNode.hh"
#include <memory>
#include <ostream>
#include <vector>

namespace blang {
struct AstRoot : public AstNode {
  void print(int indent = 0) override;
  fir::Value compile(CompilerContext *C, bool rvalue = true) override;
  void bindings(std::ostream &os) override;

  std::vector<std::shared_ptr<AstNode>> children;
};
} // namespace blang
