#include "frontend/exceptions/LvalueException/LvalueException.hh"

namespace blang {
LvalueException::LvalueException(class location loc, std::string faultyNode)
    : LocationException(loc) {
  message = faultyNode + " cannot be an lvalue";
}

[[nodiscard]] const char *LvalueException::what() const noexcept {
  return message.c_str();
}
} // namespace blang
