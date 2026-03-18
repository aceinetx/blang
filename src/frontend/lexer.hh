#pragma once
#include "frontend/token.hh"
#include "tl/expected.hpp"
#include <string>
#include <vector>

namespace blang {
class Lexer {
private:
  std::string input;
  char ch;
  size_t pos;

public:
  Lexer(std::string _input);

  tl::expected<std::vector<Token>, std::string> Tokenize();

private:
  bool IsDigit(char c);
  bool IsLetter(char c);
  bool IsIdentifierLetter(char c);

  char Peek(size_t times = 1);
  void Consume(size_t times = 1);
};
} // namespace blang
