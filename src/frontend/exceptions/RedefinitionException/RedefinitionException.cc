#include "frontend/exceptions/RedefinitionException/RedefinitionException.hh"

namespace blang {
RedefinitionException::RedefinitionException(class location loc,
                                             std::string name)
    : LocationException(loc) {
  message = "redefinition of " + name;
}

[[nodiscard]] const char *RedefinitionException::what() const noexcept {
  return message.c_str();
}
} // namespace blang
