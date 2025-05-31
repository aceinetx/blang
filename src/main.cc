#include "blang.hh"
#include <parser.hh>

using namespace blang;

int main() {
  Blang blang = Blang("b");
  blang.input = R"(
main() {
	extrn initscr, mvprintw, getch, endwin;
	initscr();
	mvprintw(0, 0, "Hello, World!");
	getch();
	endwin();
}
)";

  auto result = blang.parseAndCompile();
  if (result.is_error()) {
    fmt::println("[blang] {}", result.get_error().value());
    return 1;
  }

  blang.emit("a.out", Blang::EMIT_IR);
}
