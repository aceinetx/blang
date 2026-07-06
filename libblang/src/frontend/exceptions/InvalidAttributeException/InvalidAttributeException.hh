#pragma once
#include "frontend/exceptions/LocationException/LocationException.hh"
#include <location.hh>

namespace blang {
class InvalidAttributeException : public LocationException {
public:
  InvalidAttributeException(class location loc, std::string name);

  [[nodiscard]] const char *what() const noexcept override;

private:
  std::string message;
};
} // namespace blang
