#include "parser.hh"

using namespace blang;

void Parser::func_def(std::string name) {
  AstFuncDef *node = new AstFuncDef();
  node->name = name;

  Token token = lexer.next();
  for (;;) {
    if (token.type == Token::IDENTIFIER) {
      node->args.push_back(token.str);

      Token next = lexer.next();
      if (next.type == Token::COMMA) {
        token = lexer.next();
        continue;
      } else if (next.type == Token::RPAREN) {
        break;
      } else {
        SYNTAX_ERROR0(token, "expected one of the following: COMMA RPAREN");
      }
    } else if (token.type == Token::RPAREN) {
      break;
    } else {
      SYNTAX_ERROR0(token, "expected one of the following: IDENTIFIER");
    }
  }

  node->children = func_body();
  ERRRET();

  root->children.push_back(node);
}

std::vector<AstNode *> Parser::func_body() {
  std::vector<AstNode *> body;

  unsigned long index = 0;
  Token token = lexer.next();
  for (;;) {
    if (token.type == Token::EXTRN) {
      AstNode *node = extrn();
      ERRRET({});
      body.push_back(node);
    } else if (token.type == Token::RETURN) {
      lexer.next();
      AstNode *node = _return();
      ERRRET({});
      body.push_back(node);
    }

    if (token.type == Token::LBRACE) {
      index++;
    } else if (token.type == Token::RBRACE) {
      if (--index <= 0)
        break;
    }

    token = lexer.next();
  }

  return body;
}
