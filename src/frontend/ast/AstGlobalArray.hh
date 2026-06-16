#pragma once
#include "frontend/ast/AstNode.hh"
#include <memory>
#include <vector>

namespace blang {
struct AstGlobalArray : public AstNode {
  void print(int indent = 0) override;
  fir::Value compile(CompilerContext *C, bool rvalue) override;
  void bindings(std::ostream &os) override;

  std::string name;
  long size;
  std::vector<std::shared_ptr<AstNode>> values;
};
} // namespace blang
