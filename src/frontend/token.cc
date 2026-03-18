#include "frontend/token.hh"

blang::Token::Token(TokenType _type, TokenValue _value)
    : type(_type), value(_value) {
}
