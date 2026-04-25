#include "frontend/Driver.hh"
#include "frontend/exceptions/LexerException/LexerException.hh"
#include "parser.tab.hpp"
#include <cctype>
#include <fmt/format.h>
#include <unordered_map>

#define bounds (pos < code.length())

namespace blang {
Lexer::Lexer(std::string code) : code(std::move(code)) {
}

Parser::symbol_type Lexer::next() {
  while (bounds) {
    char c = code[pos];

    if (std::isalpha(c) || c == '_') {
      return read_identifier();
    } else if (std::isdigit(c)) {
      return read_number();
    } else if (c == '"') {
      return read_string();
    } else if (c == '\'') {
      return read_character();
    }

    auto sym = read_symbol();
    if (sym)
      return *sym;
    pos++;
  }

  Parser::location_type loc(nullptr, (int)line, (int)(line_start_pos - pos));
  return Parser::make_END(loc);
}

Parser::symbol_type Lexer::read_identifier() {
  auto loc = get_location();
  std::string identifier;

  while (bounds) {
    char c = code[pos];
    if (!(std::isalnum(c) or c == '$' or c == '_')) {
      break;
    }
    identifier.push_back(c);
    pos++;
  }

  static const std::unordered_map<
      std::string, std::function<Parser::symbol_type(Parser::location_type)>>
      keyword_mapping = {
          {"return", Parser::make_RETURN}, {"auto", Parser::make_AUTO},
          {"extrn", Parser::make_EXTRN},   {"while", Parser::make_WHILE},
          {"break", Parser::make_BREAK},   {"goto", Parser::make_GOTO},
          {"if", Parser::make_IF},         {"else", Parser::make_ELSE}};

  loc = get_loc_range(loc);
  if (keyword_mapping.contains(identifier))
    return keyword_mapping.at(identifier)(loc);

  return Parser::make_IDENTIFIER(identifier, loc);
}

Parser::symbol_type Lexer::read_number() {
  auto loc = get_location();

  long number = 0;
  constexpr long base = 10;

  while (bounds) {
    char c = code[pos];
    if (!std::isdigit(c))
      break;
    number *= base;
    number += c - '0';
    pos++;
  }

  loc = get_loc_range(loc);

  return Parser::make_NUMBER(number, loc);
}

Parser::symbol_type Lexer::read_string() {
  auto loc = get_location();

  std::string s = "";
  pos++;

  while (bounds) {
    char c = code[pos];
    if (c == '"')
      break;
    if (c == '\n')
      throw LexerException(loc, "newline in string");
    s.push_back(c);
    pos++;
  }

  if (pos >= code.length()) {
    throw LexerException(loc, "unterminated string");
  }

  pos++;

  loc = get_loc_range(loc);

  return Parser::make_STRING_LIT(s, loc);
}

Parser::symbol_type Lexer::read_character() {
  auto loc = get_location();

  pos++;
  if (!bounds) {
    throw LexerException(loc, "incomplete character literal");
  }

  char c = code[pos];

  pos++;
  if (!bounds or (bounds and code[pos] != '\'')) {
    throw LexerException(loc, "unterminated character literal");
  }
  pos++;

  return Parser::make_NUMBER((long)c, loc);
}

std::optional<Parser::symbol_type> Lexer::read_symbol() {
  char c = code[pos];
  auto loc = get_location();

  switch (c) {
  case '(':
    pos++;
    return Parser::make_LPAREN(loc);
  case ')':
    pos++;
    return Parser::make_RPAREN(loc);
  case '{':
    pos++;
    return Parser::make_LBRACE(loc);
  case '}':
    pos++;
    return Parser::make_RBRACE(loc);
  case ';':
    pos++;
    return Parser::make_SEMICOLON(loc);
  case '=':
    pos++;
    if (bounds) {
      char c = code[pos];
      auto loc2 = get_loc_range(loc);
      switch (c) {
      case '=':
        // Code ahead resolved ambiguity in '===' by making sure the first token
        // is always an ASSIGN
        pos++;
        if (bounds) {
          char c = code[pos];
          if (c == '=') {
            pos--;
            auto loc = get_location();
            loc.begin.column--;
            loc.end.column--;
            return Parser::make_ASSIGN(loc);
          }
        }
        return Parser::make_EQUAL(loc2);
      case '*':
        pos++;
        return Parser::make_ASSIGNMUL(loc2);
      case '-':
        pos++;
        return Parser::make_ASSIGNMINUS(loc2);
      case '&':
        pos++;
        return Parser::make_ASSIGNBITAND(loc2);
      }
    }
    return Parser::make_ASSIGN(loc);
  case '+':
    pos++;
    if (bounds && code[pos] == '+') {
      pos++;
      return Parser::make_PLUSPLUS(loc);
    }
    return Parser::make_PLUS(loc);
  case '-':
    pos++;
    if (bounds && code[pos] == '-') {
      pos++;
      return Parser::make_MINUSMINUS(get_loc_range(loc));
    }
    return Parser::make_MINUS(loc);
  case '*':
    pos++;
    return Parser::make_MUL(loc);
  case '/':
    pos++;
    return Parser::make_DIV(loc);
  case '%':
    pos++;
    return Parser::make_PERCENT(loc);
  case '&':
    pos++;
    return Parser::make_BITAND(loc);
  case ',':
    pos++;
    return Parser::make_COMMA(loc);
  case '>':
    pos++;
    if (pos < code.size()) {
      char c = code[pos];
      auto loc2 = get_loc_range(loc);
      if (c == '>') {
        pos++;
        return Parser::make_BITSHR(loc2);
      }
      if (c == '=') {
        pos++;
        return Parser::make_GREQ(loc2);
      }
    }
    return Parser::make_GREATER(loc);
  case '<':
    pos++;
    if (pos < code.size()) {
      char c = code[pos];
      pos++;
      auto loc2 = get_loc_range(loc);
      if (c == '<') {
        pos++;
        return Parser::make_BITSHL(loc2);
      }
      if (c == '=') {
        pos++;
        return Parser::make_LSEQ(loc2);
      }
    }
    return Parser::make_LESS(loc);
  case '!':
    pos++;
    if (bounds) {
      char c = code[pos];
      pos++;
      auto loc2 = get_loc_range(loc);
      if (c == '=')
        return Parser::make_NEQUAL(loc2);
      pos--;
    }
    return Parser::make_EXCLAMATION(loc);
  case ':':
    pos++;
    return Parser::make_COLON(loc);
  case '[':
    pos++;
    return Parser::make_LBRACKET(loc);
  case ']':
    pos++;
    return Parser::make_RBRACKET(loc);
  case '|':
    pos++;
    return Parser::make_BITOR(loc);
  }

  if (c == '\n') {
    line++;
    line_start_pos = pos;
  }

  if (!std::isspace(c)) {
    throw LexerException(loc, fmt::format("illegal char '{}'", c));
  }

  return {};
}

Parser::location_type Lexer::get_location() {
  Parser::location_type loc(nullptr, (int)line, (int)(pos - line_start_pos));
  if (line == 1)
    loc.begin.column++;
  return loc;
}

Parser::location_type Lexer::get_loc_range(Parser::location_type start,
                                           int column_offset) {
  auto end = get_location();
  start.end = end.begin;
  start.end.column += column_offset;
  return start;
}

Parser::symbol_type yylex(Driver &driver) {
  return driver.get_lexer().next();
}
} // namespace blang
