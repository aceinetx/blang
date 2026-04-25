#include "Blang.hh"
#include "frontend/DiagnosticPrinter/DiagnosticPrinter.hh"

using namespace blang;

int main(int argc, char **argv) {
  const std::string code = R"(
main(){
	extrn initscr, curs_set, clear, mvprintw, refresh, getch, endwin;
	auto k;
  auto x;
  auto y;

	initscr();
	curs_set(0);

	x = y = 1;

	while(1){
		clear();

		mvprintw(0, 0, "Hello, from B!");
		mvprintw(y, x, "@");

		refresh();

		k = getch();
		if(k == 'q'){ goto break; }
		else if(k == 'w') { y = y - 1; }
		else if(k == 's') { y = y + 1; }
		else if(k == 'a') { x = x - 1; }
		else if(k == 'd') { x = x + 1; }
	}
  break:

	endwin();

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
