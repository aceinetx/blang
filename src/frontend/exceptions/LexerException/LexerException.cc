#include "frontend/exceptions/LexerException/LexerException.hh"
#include <fmt/base.h>

namespace blang {
LexerException::LexerException(Parser::location_type loc, std::string message)
    : LocationException(loc), message(message) {
  formatted_message = fmt::format("{} at line {} column {}", message,
                                  loc.begin.line, loc.begin.column)
                          .c_str();
}

[[nodiscard]] const char *LexerException::what() const noexcept {
  return formatted_message.c_str();
}

std::string LexerException::get_simple_message() {
  return message;
}
} // namespace blang
