#include "parser.hh"

using namespace blang;

AstNode *Parser::extrn() {
  AstExtrn *node = new AstExtrn();

  Token token = lexer.next();
  for (;;) {
    if (token.type == Token::IDENTIFIER) {
      node->names.push_back(token.str);

      Token next = lexer.next();
      if (next.type == Token::COMMA) {
        token = lexer.next();
        continue;
      } else if (next.type == Token::SEMICOLON) {
        break;
      } else {
        SYNTAX_ERROR0(token, "expected one of the following: COMMA SEMICOLON");
        ERRRET(nullptr);
      }
    } else if (token.type == Token::SEMICOLON) {
      break;
    } else {
      SYNTAX_ERROR0(token, "expected one of the following: IDENTIFIER");
      ERRRET(nullptr);
    }
  }

  return node;
}
