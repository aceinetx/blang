#include "Assert.hh"
#include <fmt/format.h>

namespace blang {
AssertException::AssertException(std::string expr) {
  msg = fmt::format("assertion failed: {} == false", expr);
}

[[nodiscard]] const char *AssertException::what() const noexcept {
  return msg.c_str();
}
} // namespace blang
