#pragma once
#include "Lexer.hh"
#include "frontend/ast/AstRoot.hh"
#include <memory>
#include <string>

namespace blang {
class Driver {
public:
  explicit Driver(std::string code);
  Driver(const Driver &) = default;
  Driver(Driver &&) = delete;
  Driver &operator=(const Driver &) = default;
  Driver &operator=(Driver &&) = delete;
  ~Driver();

  const std::string &get_code();
  Lexer &get_lexer();

  void set_root(std::shared_ptr<AstRoot> new_root);
  std::shared_ptr<AstRoot> get_root();

  friend class Parser;

private:
  std::string code;
  Lexer lexer;
  std::shared_ptr<AstRoot> root;
};
} // namespace blang
