#pragma once
#include "frontend/ast/AstNode.hh"
#include <location.hh>
#include <utility>
#include <vector>

namespace blang {
struct AstIdentifierList : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;

  std::vector<std::pair<std::string, class location>> identifiers;
};
} // namespace blang
