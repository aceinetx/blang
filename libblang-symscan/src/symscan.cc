#include "libblang-symscan/symscan.hh"
#include "frontend/Lexer.hh"
#include <stdexcept>

using namespace blang;

static bool position_falls_in_range(class position &loc, class position &begin,
                                    class position &end) {
  if (loc.line == begin.line && loc.column >= begin.column) {
    return true;
  }
  if (loc.line > begin.line && loc.line < end.line) {
    return true;
  }
  if (loc.line == end.line && loc.column <= end.column) {
    return true;
  }
  return false;
}

static Parser::symbol_type scan_func(const std::string &source, Lexer &lexer,
                                     symscan::FunctionScanResult &func) {
  int brace_count = 0;

  auto brace = lexer.next();
  if (brace.kind() != Parser::symbol_kind::S_LBRACE) {
    throw std::runtime_error("Single line functions are not supported");
  } else
    brace_count++;

  for (;;) {
    auto token = lexer.next();
    switch (token.kind()) {
    case Parser::symbol_kind::S_LBRACE:
      brace_count++;
      break;
    case Parser::symbol_kind::S_RBRACE:
      brace_count--;
      if (brace_count <= 0)
        return token;
      break;
    case Parser::symbol_kind::S_AUTO: {
      for (;;) {
        auto token = lexer.next();
        if (token.kind() == Parser::symbol_kind::S_IDENTIFIER)
          func.autoSymbols.push_back(token.value.as<std::string>());
        else if (token.kind() == Parser::symbol_kind::S_SEMICOLON)
          break;
      }
    } break;
    case Parser::symbol_kind::S_EXTRN: {
      for (;;) {
        auto token = lexer.next();
        if (token.kind() == Parser::symbol_kind::S_IDENTIFIER)
          func.extrnSymbols.push_back(token.value.as<std::string>());
        else if (token.kind() == Parser::symbol_kind::S_SEMICOLON)
          break;
      }
    } break;
    default:
      break;
    }
  }
}

symscan::ScanResult
symscan::scan_source(const std::string &source,
                     std::optional<position> context_position) {
  ScanResult result{.global = {}, .functions = {}};
  Lexer lexer{source};

  for (;;) {
    auto token = lexer.next();
    switch (token.kind()) {
    case Parser::symbol_kind::S_IDENTIFIER: {
      auto symbol = token.value.as<std::string>();
      auto token = lexer.next();
      if (token.kind() == Parser::symbol_kind::S_LPAREN) {
        auto begin = token.location.begin;

        // Function definition
        FunctionScanResult func;
        for (;;) {
          auto token = lexer.next();
          if (token.kind() == Parser::symbol_kind::S_IDENTIFIER)
            func.args.push_back(token.value.as<std::string>());
          else if (token.kind() == Parser::symbol_kind::S_RPAREN)
            break;
        }
        auto end = scan_func(source, lexer, func).location.begin;

        if (context_position) {
          if (position_falls_in_range(*context_position, begin, end))
            result.focusedFunction = symbol;
        }

        result.functions[symbol] = func;
      } else {
        result.global.push_back(symbol);
      }
    };
    default:
      break;
    }
    if (token.kind() == Parser::symbol_kind::S_YYEOF)
      break;
  }

  return result;
}
