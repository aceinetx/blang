#include <utility>

#include "frontend/Driver.hh"

namespace blang {
Driver::Driver(std::string code) : code(std::move(code)), lexer(this->code) {
}

Driver::~Driver() = default;

const std::string &Driver::get_code() {
  return code;
}

Lexer &Driver::get_lexer() {
  return lexer;
}

void Driver::set_root(std::shared_ptr<AstRoot> new_root) {
  root = new_root;
}

std::shared_ptr<AstRoot> Driver::get_root() {
  return root;
}

} // namespace blang
