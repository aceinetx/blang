#include "frontend/exceptions/LocationException/LocationException.hh"
#include <fmt/base.h>

namespace blang {
LocationException::LocationException(class location location)
    : location(location) {
}

class location LocationException::get_location() {
  return location;
}

} // namespace blang
