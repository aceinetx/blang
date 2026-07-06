#pragma once
#include "frontend/exceptions/LexerException/LexerException.hh"
#include "frontend/exceptions/LocationException/LocationException.hh"
#include "frontend/exceptions/ParserException/ParserException.hh"

namespace blang {
class DiagnosticPrinter {
public:
  DiagnosticPrinter(std::string filename, std::string source);
  ~DiagnosticPrinter();

  enum { ERROR = 0, WARNING, NOTE, __DIAG_TYPE_END };

  void printSourceWithMessage(class location location, std::string message,
                              int type);
  void printDiagnostic(LexerException &exc, int type = ERROR);
  void printDiagnostic(ParserException &exc, int type = ERROR);
  void printDiagnostic(LocationException &exc, int type = ERROR);

private:
  std::string getDiagnosticTypeStringForType(int type);

  std::string filename, source;
};
}; // namespace blang
