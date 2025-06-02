#include "lexer.hh"

using namespace blang;

static bool isDigit(char c) {
  return (c >= '0' && c <= '9');
}

static bool isLetter(char c) {
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

static bool isIdentifierChar(char c) {
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) || isLetter(c);
}

Lexer::Lexer() {
  pos = 0;
}

Lexer::Lexer(std::string code) : code(code) {
  Lexer();
}

Token Lexer::number() {
  Token token = Token::create();

  token.type = Token::NUMBER;
  token.line = line;

  while (pos < code.length()) {
    char &c = code.at(pos);

    if (isDigit(c)) {
      token.number *= 10;
      token.number += c - '0';
    } else {
      break;
    }

    pos++;
  }

  return token;
}

Token Lexer::identifier() {
  Token token = Token::create();

  token.type = Token::IDENTIFIER;
  token.line = line;

  while (pos < code.length()) {
    char &c = code.at(pos);

    if (isIdentifierChar(c) || isDigit(c)) {
      token.str.push_back(c);
    } else {
      break;
    }

    pos++;
  }

  return token;
}

Token Lexer::str() {
  Token token = Token::create();

  token.type = Token::STRING;
  token.line = line;

  pos++;
  while (pos < code.length()) {
    char &c = code.at(pos);

    if (c == '"') {
      pos++;
      break;
    } else {
      token.str.push_back(c);
    }

    pos++;
  }

  return token;
}

Token Lexer::peek(unsigned long count) {
  unsigned long old_pos = pos;
  Token token;
  for (unsigned long i = 0; i < count; i++)
    token = next();
  pos = old_pos;

  return token;
}

Token Lexer::next() {
  Token token = Token::create();
  while (pos < code.length()) {
    char &c = code.at(pos);
    token = Token::create();

    if (isDigit(c)) {
      token = number();
    } else if (isLetter(c)) {
      token = identifier();
      if (token.str == "extrn")
        token.type = Token::EXTRN;
      if (token.str == "return")
        token.type = Token::RETURN;
      if (token.str == "if")
        token.type = Token::IF;
      if (token.str == "else")
        token.type = Token::ELSE;
    } else if (c == '"') {
      token = str();
    } else if (c == '\n') {
      line++;
    } else if (c == '(') {
      token.type = Token::LPAREN;
      pos++;
    } else if (c == ')') {
      token.type = Token::RPAREN;
      pos++;
    } else if (c == '{') {
      token.type = Token::LBRACE;
      pos++;
    } else if (c == '}') {
      token.type = Token::RBRACE;
      pos++;
    } else if (c == '[') {
      token.type = Token::LBRACKET;
      pos++;
    } else if (c == ']') {
      token.type = Token::RBRACKET;
      pos++;
    } else if (c == ',') {
      token.type = Token::COMMA;
      pos++;
    } else if (c == ';') {
      token.type = Token::SEMICOLON;
      pos++;
    } else if (c == '+') {
      token.type = Token::PLUS;
      pos++;
    } else if (c == '-') {
      token.type = Token::MINUS;
      pos++;
    } else if (c == '*') {
      token.type = Token::MULTIPLY;
      pos++;
    } else if (c == '/') {
      token.type = Token::DIVIDE;
      pos++;
    }

    if (token.type != Token::NONE) {
      current = token;
      return token;
    }

    pos++;
  }

  token.type = Token::END;
  current = token;
  return token;
}
