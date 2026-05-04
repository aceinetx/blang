#pragma once
#include "frontend/ast/AstNode.hh"

namespace blang {
struct AstAutoVarList : public AstNode {
  struct Entry {
    std::string identifier;
    class location identifier_location;

    std::shared_ptr<AstNode> initializer{nullptr};
    class location initializer_location{};
  };

  void print(int indent = 0) override;
  llvm::Value *compile(CompilerContext *C, bool rvalue) override;

  std::vector<Entry>::iterator begin();
  std::vector<Entry>::iterator end();

  std::vector<Entry> list{};
};
} // namespace blang
