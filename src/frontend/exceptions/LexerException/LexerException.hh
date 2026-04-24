#pragma once
#include "frontend/exceptions/LocationException/LocationException.hh"
#include "parser.tab.hpp"

namespace blang {
class LexerException : public LocationException {
public:
  LexerException(Parser::location_type loc, std::string message);

  [[nodiscard]] const char *what() const noexcept override;

  std::string get_simple_message();

private:
  std::string formatted_message;
  std::string message;
};
} // namespace blang
