#pragma once
#include "parser.tab.hpp"
#include <exception>

namespace blang {
class ParserException : public std::exception {
public:
  ParserException(Parser::location_type loc, std::string message);

  [[nodiscard]] const char *what() const noexcept override;

  Parser::location_type get_location();
  std::string get_simple_message();

private:
  std::string formatted_message;
  std::string message;
  Parser::location_type location;
};
} // namespace blang
