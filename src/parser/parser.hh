#pragma once
#include "../ast/ast.hh"
#include "../lexer/lexer.hh"
#include <pch.hh>

#define SYNTAX_ERROR0(token, msg)                                              \
  error = fmt::format("syntax error at {}: " msg, token.line);
#define SYNTAX_ERRORN(token, msg, ...)                                         \
  error = fmt::format("syntax error at {}: " msg, token.line, __VA_ARGS__);
#define ERRRET(...)                                                            \
  if (!error.empty())                                                          \
    return __VA_ARGS__;

namespace blang {
using Prec = unsigned int;

class Parser {
public:
  Lexer &lexer;
  std::unique_ptr<AstNode> root;
  std::string error;

public:
  Parser(Lexer &lexer);

  void parse();

private:
  void func_def(std::string name);
  std::vector<AstNode *> func_body();
  AstNode *extrn();

  AstNode *rvalue(Prec min_prec);
  AstNode *rvalue_atom();

  AstNode *_return();
};
} // namespace blang
