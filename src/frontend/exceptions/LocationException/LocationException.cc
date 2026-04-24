#include "frontend/exceptions/LocationException/LocationException.hh"
#include <fmt/base.h>

namespace blang {
LocationException::LocationException(Parser::location_type location)
    : location(location) {
}

Parser::location_type LocationException::get_location() {
  return location;
}

} // namespace blang
