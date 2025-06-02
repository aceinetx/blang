#include "lexer.hh"

using namespace blang;

Token Token::create() {
  Token self;
  self.type = Token::NONE;
  self.number = 0;
  return self;
}
