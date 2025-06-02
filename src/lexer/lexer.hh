#pragma once

namespace blang {
struct Token {
  enum Type {
    NONE,

    IDENTIFIER,
    EXTRN,
    RETURN,

    IF,
    ELSE,

    STRING,
    NUMBER,

    LPAREN,
    RPAREN,

    LBRACE,
    RBRACE,

    LBRACKET,
    RBRACKET,

    COMMA,
    SEMICOLON,

    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,

    END,
  };

  Type type;

  std::string str;
  int number;
  unsigned long line;

  static Token create();
};

class Lexer {
public:
  std::string code;
  Token current;

private:
  unsigned long pos, line;

public:
  Lexer();
  Lexer(std::string code);

  Token next();
  Token peek(unsigned long count = 1);

private:
  Token number();
  Token identifier();
  Token str();
};
} // namespace blang
