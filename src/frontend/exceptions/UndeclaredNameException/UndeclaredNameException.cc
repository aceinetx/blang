#include "frontend/exceptions/UndeclaredNameException/UndeclaredNameException.hh"

namespace blang {
UndeclaredNameException::UndeclaredNameException(class location loc,
                                                 std::string name)
    : LocationException(loc) {
  message = "undeclared name: " + name;
}

[[nodiscard]] const char *UndeclaredNameException::what() const noexcept {
  return message.c_str();
}
} // namespace blang
