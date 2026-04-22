#include "parser.tab.hpp"
#include <cctype>
#include <fmt/format.h>

namespace blang {
Parser::symbol_type yylex(Driver &driver) {
  // TODO: actual values in the locations
  auto &state = driver.get_lexer_state();
  auto &code = driver.get_code();

  while (state.pos < code.length()) {
    Parser::location_type loc(nullptr, 0, 0);
    char c = code[state.pos];

    if (std::isalpha(c)) {
      std::string identifier;

      while (state.pos < code.length()) {
        char c = code[state.pos];
        if (!(std::isalnum(c) or c == '$')) {
          break;
        }
        identifier.push_back(c);
        state.pos++;
      }

      if (identifier == "return")
        return Parser::make_RETURN(loc);
      if (identifier == "auto")
        return Parser::make_AUTO(loc);

      return Parser::make_IDENTIFIER(identifier, loc);
    } else if (std::isdigit(c) || c == '-') {
      long number = 0;
      constexpr long base = 10;
      const bool negative = c == '-';
      if (negative)
        state.pos++;

      while (state.pos < code.length()) {
        char c = code[state.pos];
        if (!std::isdigit(c))
          break;
        number *= base;
        number += c - '0';
        state.pos++;
      }

      return Parser::make_NUMBER(negative ? -number : number, loc);
    }

    switch (c) {
    case '(':
      state.pos++;
      return Parser::make_LPAREN(loc);
    case ')':
      state.pos++;
      return Parser::make_RPAREN(loc);
    case '{':
      state.pos++;
      return Parser::make_LBRACE(loc);
    case '}':
      state.pos++;
      return Parser::make_RBRACE(loc);
    case ';':
      state.pos++;
      return Parser::make_SEMICOLON(loc);
    }

    if (!std::isspace(c)) {
      throw std::runtime_error(fmt::format("lexer error: illegal char {}", c));
    }
    state.pos++;
  }

  Parser::location_type loc(nullptr, 0, 0);
  return Parser::make_END(loc);
}
} // namespace blang
