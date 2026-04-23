#pragma once
#include <exception>
#include <fmt/base.h>
#include <string>

namespace blang {
class AssertException : public std::exception {
public:
  AssertException(std::string expr);

  [[nodiscard]] const char *what() const noexcept override;

private:
  std::string msg;
};
} // namespace blang

#define blangassert(expr)                                                      \
  if (!(!!(expr)))                                                             \
    throw AssertException(#expr);
