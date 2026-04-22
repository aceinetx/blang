#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
main() { 
	extrn initscr;
	extrn mvprintw;
	extrn getch;
	extrn endwin;
	initscr();
	mvprintw(0, 0, "Hello, World! %p", mvprintw);
	getch();
	endwin();
	return(endwin); 
})");
}
