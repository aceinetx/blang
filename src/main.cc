#include "blang.hh"
#include <parser.hh>

using namespace blang;

int main() {
  Blang blang = Blang("b");
  if (!blang.target)
    return 1;

  blang.input = R"(
main() {
	extrn initscr, clear, mvprintw, refresh, getch, endwin, curs_set;

	auto x, y, k;
	x = y = 1;
	k = 0;

	auto QUIT, UP, DOWN, LEFT, RIGHT;
	// initalize keys
	QUIT = 113;
	UP = 119;
	DOWN = 115;
	LEFT = 97;
	RIGHT = 100;

	initscr();
	curs_set(0);

	while(k != QUIT) {
		clear();

		mvprintw(0, 0, "Hello ncurses, from B!");
		mvprintw(y, x, "@");

		refresh();
		
		k = getch();

		if(k == UP){
			y -= 1;
		} else if(k == DOWN){
			y += 1;
		} else if(k == LEFT){
			x -= 1;
		} else if(k == RIGHT){
			x += 1;
		}
	}

	endwin();
}
)";

  auto result = blang.parseAndCompile();
  if (result.is_error()) {
    fmt::println("blang: {}", result.get_error().value());
    return 1;
  }

  blang.emit("a.out", Blang::EMIT_IR);
}
