#pragma once
#include <istream>
#include <memory>

namespace yy {
class parser;
}

class Lexer {
public:
  explicit Lexer(std::istream &in);

  int yylex(yy::parser::semantic_type *yylval);

private:
  std::istream &input;
};
