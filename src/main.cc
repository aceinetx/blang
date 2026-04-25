#include "Blang.hh"
#include "frontend/DiagnosticPrinter/DiagnosticPrinter.hh"

using namespace blang;

int main(int argc, char **argv) {
  const std::string code = R"(
main() {
  auto x;
  x = 5 << 2 | 3 >> 10 << 5 & 5 << 10;
  return(*&x);
}
  )";

  Blang blang = Blang("b");
  DiagnosticPrinter diag_printer = DiagnosticPrinter("b", code);
  try {
    blang.compile(code);
  } catch (LexerException &exc) {
    diag_printer.printDiagnostic(exc);
  } catch (ParserException &exc) {
    diag_printer.printDiagnostic(exc);
  } catch (LocationException &exc) {
    diag_printer.printDiagnostic(exc);
  }
}
