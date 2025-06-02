#include "parser.hh"

using namespace blang;

enum class Assoc {
  LEFT,
  RIGHT,
};

static Prec min_prec = 1;
static std::map<Token::Type, std::pair<Prec, Assoc>> opinfo_map = {
    {Token::PLUS, {1, Assoc::LEFT}},
    {Token::MINUS, {1, Assoc::LEFT}},
    {Token::MULTIPLY, {2, Assoc::LEFT}},
    {Token::DIVIDE, {2, Assoc::LEFT}},
};

static bool isBinop(const Token &token) {
  return (token.type == Token::PLUS) || (token.type == Token::MINUS) ||
         (token.type == Token::MULTIPLY) || (token.type == Token::DIVIDE);
}

AstNode *Parser::rvalue_atom() {
  Token tok = lexer.current;

  if (tok.type == Token::LPAREN) {
    lexer.next();
    AstNode *node = rvalue(1);
    if (lexer.current.type != Token::RPAREN) {
      SYNTAX_ERROR0(lexer.current, "unmatched '('");
      return nullptr;
    }
    lexer.next();
    return node;
  } else if (tok.type == Token::END) {
    SYNTAX_ERROR0(tok, "unexpected EOF");
    return nullptr;
  } else if (isBinop(tok)) {
    SYNTAX_ERROR0(tok, "expected an atom, got a binop-kind token");
    return nullptr;
  } else {
    if (tok.type != Token::NUMBER) {
      SYNTAX_ERROR0(tok, "expected a number");
      return nullptr;
    }

    lexer.next();

    AstNumber *node = new AstNumber(tok.number);
    return node;
  }
}

AstNode *Parser::rvalue(Prec min_prec) {
  AstNode *lhs;
  AstBinaryOp *lhs_inner;

  lhs = Parser::rvalue_atom();
  ERRRET(nullptr);

  while (true) {
    if (!isBinop(lexer.current) ||
        opinfo_map[lexer.current.type].first < min_prec) {
      break;
    }

    if (!isBinop(lexer.current)) {
      SYNTAX_ERROR0(lexer.current, "expected binop token");
      return nullptr;
    }

    Token::Type op = lexer.current.type;
    Prec prec = opinfo_map[lexer.current.type].first;
    Assoc assoc = opinfo_map[lexer.current.type].second;
    Prec next_min_prec;
    if (assoc == Assoc::LEFT)
      next_min_prec = prec + 1;
    else
      next_min_prec = prec;

    lexer.next();

    AstNode *rhs = Parser::rvalue(next_min_prec);
    ERRRET(nullptr);

    lhs_inner = new AstBinaryOp();
    lhs_inner->left = lhs;
    lhs_inner->right = rhs;

    // TODO: Comparison operators
    switch (op) {
    case Token::PLUS:
      lhs_inner->op = "add";
      break;
    case Token::MINUS:
      lhs_inner->op = "sub";
      break;
    case Token::MULTIPLY:
      lhs_inner->op = "mul";
      break;
    case Token::DIVIDE:
      lhs_inner->op = "div";
      break;
    default:
      break;
    }

    lhs = lhs_inner;
  }

  return lhs;
}
