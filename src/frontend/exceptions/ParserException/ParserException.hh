#pragma once
#include "frontend/exceptions/LocationException/LocationException.hh"
#include "location.hh"

namespace blang {
class ParserException : public LocationException {
public:
  ParserException(class location loc, std::string message);

  [[nodiscard]] const char *what() const noexcept override;

  std::string get_simple_message();

private:
  std::string formatted_message;
  std::string message;
};
} // namespace blang
