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
  std::optional<Parser::symbol_type> read_symbol();
  Parser::location_type get_location();
  Parser::location_type get_loc_range(Parser::location_type start,
                                      int column_offset = -1);
  std::string parse_escape_sequences(const std::string &s);

  std::size_t pos = 0;
  std::size_t line = 1;
  std::size_t line_start_pos = 0;
  std::string code;
};
} // namespace blang
