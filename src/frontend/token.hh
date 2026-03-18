#pragma once
#include <optional>
#include <variant>

namespace blang {
using TokenValue = std::optional<std::variant<long, std::string>>;
enum class TokenType {
  Return,
  Auto,
  Extrn,
  If,
  Else,
  While,
  Break,
  Identifier,
  StringLiteral,
  Number,

  Increment,
  Decrement,
  BitshlAssign,
  BitshrAssign,
  BitandAssign,
  BitorAssign,
  PlusAssign,
  MinusAssign,
  MultAssign,
  DivAssign,
  Equal,
  Assign,
  Plus,
  Minus,
};

struct Token {
  TokenType type;
  TokenValue value;

  Token(TokenType type, TokenValue value);
};
} // namespace blang
