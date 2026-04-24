#pragma once
#include "frontend/LexerException.hh"
#include "frontend/ParserException.hh"

namespace blang {
class DiagnosticPrinter {
public:
	DiagnosticPrinter(std::string filename, std::string source);
	~DiagnosticPrinter();

  enum {
    ERROR = 0,
    WARNING,
    NOTE,
    __DIAG_TYPE_END
  };

  void printSourceWithMessage(Parser::location_type location, std::string message, int type);
	void printDiagnostic(LexerException& exc);
	void printDiagnostic(ParserException& exc);

private:
  std::string getDiagnosticTypeStringForType(int type);

	std::string filename, source;
};
};
