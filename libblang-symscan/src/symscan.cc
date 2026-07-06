#include "libblang-symscan/symscan.hh"
#include "frontend/Lexer.hh"
#include <stdexcept>

using namespace blang;

static void scan_func(const std::string &source, Lexer &lexer,
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
        return;
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

symscan::ScanResult symscan::scan_source(const std::string &source) {
  ScanResult result{.global = {}, .functions = {}};
  Lexer lexer{source};

  for (;;) {
    auto token = lexer.next();
    switch (token.kind()) {
    case Parser::symbol_kind::S_IDENTIFIER: {
      auto symbol = token.value.as<std::string>();
      auto token = lexer.next();
      if (token.kind() == Parser::symbol_kind::S_LPAREN) {
        // Function definition
        FunctionScanResult func;
        for (;;) {
          auto token = lexer.next();
          if (token.kind() == Parser::symbol_kind::S_IDENTIFIER)
            func.args.push_back(token.value.as<std::string>());
          else if (token.kind() == Parser::symbol_kind::S_RPAREN)
            break;
        }
        scan_func(source, lexer, func);

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
