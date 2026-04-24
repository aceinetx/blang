#pragma once
#include "frontend/exceptions/LocationException/LocationException.hh"
#include <location.hh>

namespace blang {
class UnresolvedLabelException : public LocationException {
public:
  UnresolvedLabelException(class location loc, std::string name);

  [[nodiscard]] const char *what() const noexcept override;

private:
  std::string message;
};
} // namespace blang
