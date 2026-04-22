#include "Blang.hh"
#include "parser.tab.hpp"
#include <fmt/base.h>

namespace blang {
Blang::Blang() {
  Driver driver = Driver("main() { return(0); }");
  Parser parser = Parser(driver);
  fmt::println("{}", parser.parse());
  driver.get_root()->print();
};

Blang::~Blang() = default;
} // namespace blang
