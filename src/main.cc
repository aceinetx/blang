#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
main(argc, argv) {
	extrn putchar;
	if(argc == 1){
		putchar('1');
	} else if(argc == 2){
		putchar('2');
	} else if(argc == 3){
		putchar('3');
	} else {
		putchar('o');
	}
	putchar(10);
	return(!!0);
}
)");
}
