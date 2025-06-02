#include "parser.hh"

using namespace blang;

AstNode *Parser::_return() {
  AstReturn *node = new AstReturn();
  node->expr = rvalue(1);
  ERRRET(nullptr);

  return node;
}
