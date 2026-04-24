#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
main(argc, argv) {
	extrn putchar;
	goto end;
	putchar(69);
	putchar(10);
	end:
	return(0);
}
)");
}
