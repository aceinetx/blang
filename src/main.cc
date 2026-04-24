#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
main(argc, argv) {
	extrn printf, putchar;
	while(argc > 0){
		printf("%d", argc);
		argc = argc - 1;
	}
	putchar(10);
	return(0);
}
)");
}
