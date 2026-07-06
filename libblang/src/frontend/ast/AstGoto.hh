#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstGoto : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;

  std::string name = "";
  class location label_symbol_location;
};
} // namespace blang
