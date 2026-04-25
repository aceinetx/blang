#include "Blang.hh"
#include "frontend/DiagnosticPrinter/DiagnosticPrinter.hh"

using namespace blang;

int main(int argc, char **argv) {
  const std::string code = R"(
main(argc, argv){
  extrn puts;
  auto x;
  x = 3;
  if(argc == 1) 
    if(x == 1)
      puts("1");
    else if(x == 2)
      puts("2");
    else
      puts("x other");
  else
    puts("argc other");
    

  return(0);
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
