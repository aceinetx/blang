#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstGlobalArray : public AstNode {
  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;

  std::string name;
  long size;
  std::vector<std::shared_ptr<AstNode>> values;
};
} // namespace blang
