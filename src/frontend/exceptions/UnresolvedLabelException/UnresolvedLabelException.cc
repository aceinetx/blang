#include "frontend/exceptions/UnresolvedLabelException/UnresolvedLabelException.hh"

namespace blang {
UnresolvedLabelException::UnresolvedLabelException(class location loc,
                                                   std::string name)
    : LocationException(loc) {
  message = "unresolved label: " + name;
}

[[nodiscard]] const char *UnresolvedLabelException::what() const noexcept {
  return message.c_str();
}
} // namespace blang
