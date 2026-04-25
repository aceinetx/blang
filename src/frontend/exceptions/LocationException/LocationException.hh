#pragma once
#include "location.hh"
#include <exception>

namespace blang {
class LocationException : public std::exception {
public:
  LocationException(class location location);

  class location get_location();

private:
  class location location;
};
} // namespace blang
