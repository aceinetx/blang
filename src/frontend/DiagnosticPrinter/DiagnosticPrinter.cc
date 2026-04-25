#include "frontend/DiagnosticPrinter/DiagnosticPrinter.hh"
#include <fmt/base.h>

#include <utility>

#define WHITE_BOLD "\033[1;37m"
#define RED_BOLD "\033[1;31m"
#define GREEN_BOLD "\033[1;32m"
#define PURPLE_BOLD "\033[1;35m"
#define CYAN_BOLD "\033[1;36m"
#define GREEN "\033[0;92m"
#define RESET "\033[0m"

static std::string replace_all(const std::string &str, const std::string &from,
                               const std::string &to) {
  if (from.empty())
    return str;

  std::string result = str;
  size_t start_pos = 0;
  while ((start_pos = result.find(from, start_pos)) != std::string::npos) {
    result.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return result;
}

static std::vector<std::string> string_split(std::string s,
                                             const std::string &delimiter) {
  std::vector<std::string> tokens;
  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    token = s.substr(0, pos);
    tokens.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  tokens.push_back(s);

  return tokens;
}

namespace blang {
DiagnosticPrinter::DiagnosticPrinter(std::string filename, std::string source)
    : filename(std::move(filename)), source(std::move(source)) {
}

DiagnosticPrinter::~DiagnosticPrinter() = default;

void DiagnosticPrinter::printSourceWithMessage(class location location,
                                               std::string message, int type) {
  std::string diagnostic_type_string = getDiagnosticTypeStringForType(type);

  // Print the message at the top
  fmt::println(WHITE_BOLD "{}:{}:{}: {} " WHITE_BOLD "{}" RESET, filename,
               location.begin.line, location.begin.column,
               diagnostic_type_string, message);

  // Print the source code line
  auto lines = string_split(source, "\n");

  if (lines.size() < (size_t)location.begin.line) {
    fmt::println("(no source available)");
    return;
  }

  /* print the source code */
  std::string tab = "        ";
  std::string tab_strip = tab;
  tab_strip[0] = '|';

  auto line = lines[location.begin.line - 1];

  fmt::println("{: 5} | {}", location.begin.line,
               replace_all(line, "\t", tab_strip));
  fmt::print("      | ");

  for (int i = 0; i < location.begin.column - 1; i++) {
    if (line[i] == '\t') {
      fmt::print("{}", tab);
    } else {
      fmt::print(" ");
    }
  }
  fmt::print(GREEN "^\n" RESET);
}

void DiagnosticPrinter::printDiagnostic(LexerException &exc, int type) {
  printSourceWithMessage(exc.get_location(), exc.get_simple_message(), type);
}

void DiagnosticPrinter::printDiagnostic(ParserException &exc, int type) {
  printSourceWithMessage(exc.get_location(), exc.get_simple_message(), type);
}

void DiagnosticPrinter::printDiagnostic(LocationException &exc, int type) {
  printSourceWithMessage(exc.get_location(), exc.what(), type);
}

std::string DiagnosticPrinter::getDiagnosticTypeStringForType(int type) {
  type = type % __DIAG_TYPE_END;
  switch (type) {
  case ERROR:
    return RED_BOLD "error:" RESET;
  case WARNING:
    return PURPLE_BOLD "warning:" RESET;
  case NOTE:
    return CYAN_BOLD "note:" RESET;
  }
  return "";
}
} // namespace blang
