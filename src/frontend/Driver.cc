#include <utility>

#include "frontend/Driver.hh"

namespace blang {
Driver::Driver(std::string code) : code(std::move(code)) {
}

Driver::~Driver() = default;

const std::string &Driver::get_code() {
  return code;
}

LexerState &Driver::get_lexer_state() {
  return lexer_state;
}

void Driver::set_root(std::shared_ptr<AstRoot> new_root) {
  root = new_root;
}

std::shared_ptr<AstRoot> Driver::get_root() {
  return root;
}

} // namespace blang
