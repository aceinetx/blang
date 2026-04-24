#include "frontend/LexerException.hh"
#include <fmt/base.h>

namespace blang {
LexerException::LexerException(Parser::location_type loc, std::string message) : message(message), location(loc) {
  formatted_message = fmt::format("{} at line {} column {}", message,
                                  loc.begin.line, loc.begin.column)
                          .c_str();
}

[[nodiscard]] const char *LexerException::what() const noexcept {
  return formatted_message.c_str();
}


Parser::location_type LexerException::get_location(){
  return location;
}

std::string LexerException::get_simple_message(){
  return message;
}
} // namespace blang
