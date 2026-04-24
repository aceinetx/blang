#include "frontend/Driver.hh"
#include "frontend/LexerException.hh"
#include "parser.tab.hpp"
#include <cctype>
#include <fmt/format.h>
#include <unordered_map>

#define bounds (pos < code.length())

namespace blang {
Lexer::Lexer(std::string code) : code(std::move(code)) {
}

Parser::symbol_type Lexer::next() {
  while (bounds) {
    auto loc = get_location();
    char c = code[pos];

    if (std::isalpha(c)) {
      return read_identifier();
    } else if (std::isdigit(c) || c == '-') {
      return read_number();
    } else if (c == '"') {
      return read_string();
    } else if (c == '\'') {
      return read_character();
    }

    pos++;
    switch (c) {
    case '(':
      return Parser::make_LPAREN(loc);
    case ')':
      return Parser::make_RPAREN(loc);
    case '{':
      return Parser::make_LBRACE(loc);
    case '}':
      return Parser::make_RBRACE(loc);
    case ';':
      return Parser::make_SEMICOLON(loc);
    case '=':
      if (pos < code.size()) {
        char c = code[pos];
        pos++;
        switch (c) {
        case '=':
          return Parser::make_EQUAL(loc);
        case '!':
          return Parser::make_NEQUAL(loc);
        case '>':
          return Parser::make_GREQ(loc);
        case '<':
          return Parser::make_LSEQ(loc);
        }
        pos--;
      }
      return Parser::make_ASSIGN(loc);
    case '+':
      return Parser::make_PLUS(loc);
    case '-':
      return Parser::make_MINUS(loc);
    case '*':
      return Parser::make_MUL(loc);
    case '/':
      return Parser::make_DIV(loc);
    case '&':
      return Parser::make_AMPERSAND(loc);
    case ',':
      return Parser::make_COMMA(loc);
    case '>':
      return Parser::make_GREATER(loc);
    case '<':
      return Parser::make_LESS(loc);
    case '!':
      return Parser::make_EXCLAMATION(loc);
    case ':':
      return Parser::make_COLON(loc);
    case '[':
      return Parser::make_LBRACKET(loc);
    case ']':
      return Parser::make_RBRACKET(loc);
    }
    pos--;

    if (c == '\n') {
      line++;
      line_start_pos = pos;
    }

    if (!std::isspace(c)) {
      throw LexerException(loc, fmt::format("illegal char '{}'", c));
    }
    pos++;
  }

  Parser::location_type loc(nullptr, (int)line, (int)(line_start_pos - pos));
  return Parser::make_END(loc);
}

Parser::symbol_type Lexer::read_identifier() {
  auto loc = get_location();
  std::string identifier;

  while (bounds) {
    char c = code[pos];
    if (!(std::isalnum(c) or c == '$')) {
      break;
    }
    identifier.push_back(c);
    pos++;
  }

  static const std::unordered_map<
      std::string, std::function<Parser::symbol_type(Parser::location_type)>>
      keyword_mapping = {
          {"return", Parser::make_RETURN}, {"auto", Parser::make_AUTO},
          {"extrn", Parser::make_EXTRN},   {"while", Parser::make_WHILE},
          {"goto", Parser::make_GOTO},     {"if", Parser::make_IF},
          {"else", Parser::make_ELSE}};

  if (keyword_mapping.contains(identifier))
    return keyword_mapping.at(identifier)(loc);

  return Parser::make_IDENTIFIER(identifier, loc);
}

Parser::symbol_type Lexer::read_number() {
  auto loc = get_location();

  char c = code[pos];
  long number = 0;
  constexpr long base = 10;
  const bool negative = c == '-';
  if (negative) {
    pos++;
    if (bounds and !std::isdigit(code[pos])) {
      return Parser::make_MINUS(loc);
    }
  }

  while (bounds) {
    char c = code[pos];
    if (!std::isdigit(c))
      break;
    number *= base;
    number += c - '0';
    pos++;
  }

  return Parser::make_NUMBER(negative ? -number : number, loc);
}

Parser::symbol_type Lexer::read_string() {
  auto loc = get_location();

  std::string s = "";
  pos++;

  while (bounds) {
    char c = code[pos];
    if (c == '"')
      break;
    if (c == '\n')
      throw LexerException(loc, "newline in string");
    s.push_back(c);
    pos++;
  }

  if (pos >= code.length()) {
    throw LexerException(loc, "unterminated string");
  }

  pos++;

  return Parser::make_STRING_LIT(s, loc);
}

Parser::symbol_type Lexer::read_character() {
  auto loc = get_location();

  pos++;
  if (!bounds) {
    throw LexerException(loc, "incomplete character literal");
  }

  char c = code[pos];

  pos++;
  if (!bounds or (bounds and code[pos] != '\'')) {
    throw LexerException(loc, "unterminated character literal");
  }
  pos++;

  return Parser::make_NUMBER((long)c, loc);
}

Parser::location_type Lexer::get_location() {
  Parser::location_type loc(nullptr, (int)line, (int)(pos - line_start_pos));
  return loc;
}

Parser::symbol_type yylex(Driver &driver) {
  return driver.get_lexer().next();
}
} // namespace blang
