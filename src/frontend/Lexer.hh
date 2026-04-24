#pragma once
#include <utility>

#include "parser.tab.hpp"

namespace blang {
class Lexer {
public:
  Lexer(std::string code);

  Parser::symbol_type next();

private:
  Parser::symbol_type read_identifier();
  Parser::symbol_type read_number();
  Parser::symbol_type read_string();
  Parser::symbol_type read_character();
  Parser::location_type get_location();

  std::size_t pos = 0;
  std::size_t line = 1;
  std::size_t line_start_pos = 0;
  std::string code;
};
} // namespace blang
