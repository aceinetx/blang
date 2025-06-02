#include "parser.hh"

using namespace blang;

Parser::Parser(Lexer &lexer)
    : lexer(lexer), root(std::make_unique<AstRootNode>()) {
}

void Parser::parse() {
  Token token = lexer.next();
  while (token.type != Token::END) {
    if (token.type == Token::IDENTIFIER) {
      if (lexer.peek().type == Token::LPAREN) {
        lexer.next();
        func_def(token.str);
        ERRRET();
      } else {
        SYNTAX_ERROR0(token, "expected one of the following: LPAREN");
        ERRRET();
      }
    }
    token = lexer.next();
  }
}
