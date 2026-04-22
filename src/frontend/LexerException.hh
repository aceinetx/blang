#pragma once
#include "parser.tab.hpp"
#include <exception>

namespace blang {
class LexerException : public std::exception {
public:
  LexerException(Parser::location_type loc, std::string message);

  [[nodiscard]] const char *what() const noexcept override;

private:
  std::string formatted_message;
};
} // namespace blang
