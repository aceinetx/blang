#include "frontend/LexerException.hh"
#include "parser.tab.hpp"
#include <cctype>
#include <fmt/format.h>

namespace blang {
Parser::symbol_type yylex(Driver &driver) {
  auto &state = driver.get_lexer_state();
  auto &code = driver.get_code();

#define bounds (state.pos < code.length())

  while (bounds) {
    Parser::location_type loc(nullptr, (int)state.line,
                              (int)(state.pos - state.line_start_pos));
    char c = code[state.pos];

    if (std::isalpha(c)) {
      std::string identifier;

      while (bounds) {
        char c = code[state.pos];
        if (!(std::isalnum(c) or c == '$')) {
          break;
        }
        identifier.push_back(c);
        state.pos++;
      }

      /* Keywords */
      if (identifier == "return")
        return Parser::make_RETURN(loc);
      if (identifier == "auto")
        return Parser::make_AUTO(loc);
      if (identifier == "extrn")
        return Parser::make_EXTRN(loc);
      if (identifier == "while")
        return Parser::make_WHILE(loc);
      if (identifier == "goto")
        return Parser::make_GOTO(loc);
      if (identifier == "if")
        return Parser::make_IF(loc);
      if (identifier == "else")
        return Parser::make_ELSE(loc);

      return Parser::make_IDENTIFIER(identifier, loc);
    } else if (std::isdigit(c) || c == '-') {
      long number = 0;
      constexpr long base = 10;
      const bool negative = c == '-';
      if (negative) {
        state.pos++;
        if (bounds and !std::isdigit(code[state.pos])) {
          return Parser::make_MINUS(loc);
        }
      }

      while (bounds) {
        char c = code[state.pos];
        if (!std::isdigit(c))
          break;
        number *= base;
        number += c - '0';
        state.pos++;
      }

      return Parser::make_NUMBER(negative ? -number : number, loc);
    } else if (c == '"') {
      std::string s = "";
      state.pos++;

      while (bounds) {
        char c = code[state.pos];
        if (c == '"')
          break;
        if (c == '\n')
          throw LexerException(loc, "newline in string");
        s.push_back(c);
        state.pos++;
      }

      if (state.pos >= code.length()) {
        throw LexerException(loc, "unterminated string");
      }

      state.pos++;

      return Parser::make_STRING_LIT(s, loc);
    } else if (c == '\'') {
      state.pos++;
      if (!bounds) {
        throw LexerException(loc, "incomplete character literal");
      }

      char c = code[state.pos];

      state.pos++;
      if (!bounds or (bounds and code[state.pos] != '\'')) {
        throw LexerException(loc, "unterminated character literal");
      }
      state.pos++;

      return Parser::make_NUMBER((long)c, loc);
    }

    state.pos++;
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
      if (state.pos < code.size()) {
        char c = code[state.pos];
        state.pos++;
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
        state.pos--;
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
    state.pos--;

    if (c == '\n') {
      state.line++;
      state.line_start_pos = state.pos;
    }

    if (!std::isspace(c)) {
      throw LexerException(loc, fmt::format("illegal char '{}'", c));
    }
    state.pos++;
  }

  Parser::location_type loc(nullptr, (int)state.line,
                            (int)(state.line_start_pos - state.pos));
  return Parser::make_END(loc);
}
} // namespace blang
