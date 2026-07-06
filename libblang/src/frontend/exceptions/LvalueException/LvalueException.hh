#pragma once
#include "frontend/exceptions/LocationException/LocationException.hh"
#include <location.hh>

namespace blang {
class LvalueException : public LocationException {
public:
  LvalueException(class location loc, std::string faultyNode);

  [[nodiscard]] const char *what() const noexcept override;

private:
  std::string message;
};
} // namespace blang
