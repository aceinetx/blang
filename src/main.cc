#include "Blang.hh"
#include "frontend/DiagnosticPrinter/DiagnosticPrinter.hh"

using namespace blang;

int main(int argc, char **argv) {
  const std::string code = R"(
main(argc, argv) {
	auto x;
	return(x);
}
  )";

  std::vector<std::string> srcs = {
    R"(
main(argc, argv) {
	auto x;
	return(x a);
}
    )",
    R"(
main(argc argv) {
	auto x;
	return(x);
}
    )",
    R"(
main(argc .argv) {
	auto x;
	return(x);
}
    )",
  };
  
  for(auto& code : srcs){
    Blang blang = Blang("b");
    DiagnosticPrinter diag_printer = DiagnosticPrinter("b", code);
    try {
      blang.compile(code);
    } catch (LexerException& exc){
      diag_printer.printDiagnostic(exc);
    } catch (ParserException& exc){
      diag_printer.printDiagnostic(exc);
    }
  }
}
