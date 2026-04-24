#pragma once
#include "parser.tab.hpp"
#include <exception>

namespace blang {
class LocationException : public std::exception {
public:
  LocationException(Parser::location_type location);

  Parser::location_type get_location();

private:
  Parser::location_type location;
};
} // namespace blang
