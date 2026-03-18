#pragma once
#include "ast/ast.hh"
#include "frontend/token.hh"

namespace blang {
struct Driver {
  std::shared_ptr<blang::AstNode> root;
  std::vector<Token> tokens;
  std::size_t tokenIndex;
  std::string error;
};
} // namespace blang
