#pragma once
#include "fircpp/block.hpp"
#include "frontend/ast/AstNode.hh"
#include <memory>

namespace blang {
struct AstSwitch : public AstNode {
  void print(int indent = 0) override;
  fir::Value compile(CompilerContext *C, bool rvalue) override;

  void add_case(CompilerContext *C, long number, fir::Block block);

  std::shared_ptr<AstNode> expression;
  std::shared_ptr<AstNode> statement;

  fir::Block end;
  fir::Block body;
  fir::Block evaluator;
  fir::Value value;
};
} // namespace blang
