#include "frontend/exceptions/InvalidAttributeException/InvalidAttributeException.hh"

namespace blang {
InvalidAttributeException::InvalidAttributeException(class location loc,
                                                     std::string name)
    : LocationException(loc) {
  message = "invalid attribute: " + name;
}

[[nodiscard]] const char *InvalidAttributeException::what() const noexcept {
  return message.c_str();
}
} // namespace blang
