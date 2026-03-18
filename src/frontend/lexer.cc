#include "frontend/lexer.hh"
#include <fmt/base.h>

blang::Lexer::Lexer(std::string _input) : input(_input), ch(0), pos(0) {
  if (!input.empty()) {
    ch = input[0];
  }
}

tl::expected<std::vector<blang::Token>, std::string> blang::Lexer::Tokenize() {
  std::vector<blang::Token> tokens;

  auto keywordMapping = std::map<std::string, TokenType>({
      {"return", TokenType::Return},
      {"auto", TokenType::Auto},
      {"extrn", TokenType::Extrn},
      {"if", TokenType::If},
      {"else", TokenType::Else},
      {"while", TokenType::While},
      {"break", TokenType::Break},
  });

  while (pos < input.size()) {
    if (IsDigit(ch) || ch == '-') {
      if (ch == '-') {
        Consume();
        if (!IsDigit(ch))
          return tl::unexpected(std::string("expected a digit after -"));
      }

      long num = 0;
      while (IsDigit(ch)) {
        num *= 10;
        num += ch - '0';
        Consume();
      }
      tokens.push_back(Token(TokenType::Number, num));
    } else if (IsLetter(ch)) {
      std::string s;
      while (IsIdentifierLetter(ch)) {
        s.push_back(ch);
        Consume();
      }

      if (keywordMapping.contains(s)) {
        tokens.push_back(Token(keywordMapping[s], {}));
      } else
        tokens.push_back(Token(TokenType::Identifier, s));
    } else if (ch == '"') {
      Consume();

      std::string s;
      while (ch != '"') {
        s.push_back(ch);
        Consume();
      }
      tokens.push_back(Token(TokenType::StringLiteral, s));
      Consume();
    } else if (ch == '+') {
      if (Peek() == '+')
        tokens.push_back(Token(TokenType::Increment, {}));
      else
        tokens.push_back(Token(TokenType::Plus, {}));
    } else if (ch == '-') {
      if (Peek() == '-')
        tokens.push_back(Token(TokenType::Decrement, {}));
      else
        tokens.push_back(Token(TokenType::Minus, {}));
    } else if (ch == '=') {
      switch (Peek()) {
      case '<':
        if (Peek(2) != '<')
          return tl::unexpected("expected <");
        Consume(3);
        tokens.push_back(Token(TokenType::BitshlAssign, {}));
        break;
      case '>':
        if (Peek(2) != '>')
          return tl::unexpected("expected >");
        Consume(3);
        tokens.push_back(Token(TokenType::BitshrAssign, {}));
        break;
      case '&':
        Consume(2);
        tokens.push_back(Token(TokenType::BitandAssign, {}));
        break;
      case '|':
        Consume(2);
        tokens.push_back(Token(TokenType::BitorAssign, {}));
        break;
      case '+':
        Consume(2);
        tokens.push_back(Token(TokenType::PlusAssign, {}));
        break;
      case '-':
        Consume(2);
        tokens.push_back(Token(TokenType::MinusAssign, {}));
        break;
      case '*':
        Consume(2);
        tokens.push_back(Token(TokenType::MultAssign, {}));
        break;
      case '/':
        Consume(2);
        tokens.push_back(Token(TokenType::DivAssign, {}));
        break;
      case '=':
        Consume(2);
        tokens.push_back(Token(TokenType::Equal, {}));
        break;
      default:
        tokens.push_back(Token(TokenType::Assign, {}));
        Consume();
      }
    } else if (ch == ' ' || ch == '\n')
      Consume();
    else {
      return tl::unexpected(fmt::format("unknown char: {}", ch));
    }
  }

  /*
  for (const Token& token : tokens) {
    fmt::print("{}\n", (uint8_t)token.type);
  }
    */

  return tokens;
}

bool blang::Lexer::IsDigit(char c) {
  return ((c >= '0') && (c <= '9'));
}

bool blang::Lexer::IsLetter(char c) {
  return (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')));
}

bool blang::Lexer::IsIdentifierLetter(char c) {
  return IsLetter(c) || IsDigit(c) || c == '_' || c == '$';
}

char blang::Lexer::Peek(size_t times) {
  size_t peek_pos = pos + times;
  return peek_pos >= input.size() ? 0 : input[peek_pos];
}

void blang::Lexer::Consume(size_t times) {
  for ([[maybe_unused]] size_t i = 0; i < times; i++) {
    pos++;
    ch = pos < input.size() ? input[pos] : 0;
  }
}
