#include "frontend/ParserException.hh"
#include <fmt/base.h>

namespace blang {
ParserException::ParserException(Parser::location_type loc, std::string message) : message(message), location(loc) {
  formatted_message = fmt::format("{} at line {} column {}", message,
                                  loc.begin.line, loc.begin.column)
                          .c_str();
}

[[nodiscard]] const char *ParserException::what() const noexcept {
  return formatted_message.c_str();
}


Parser::location_type ParserException::get_location(){
  return location;
}

std::string ParserException::get_simple_message(){
  return message;
}
} // namespace blang
