#include "frontend/ParserException.hh"
#include <fmt/base.h>

namespace blang {
ParserException::ParserException(Parser::location_type loc, std::string message)
    : LocationException(loc), message(message) {
  formatted_message = fmt::format("{} at line {} column {}", message,
                                  loc.begin.line, loc.begin.column)
                          .c_str();
}

[[nodiscard]] const char *ParserException::what() const noexcept {
  return formatted_message.c_str();
}

std::string ParserException::get_simple_message() {
  return message;
}
} // namespace blang
