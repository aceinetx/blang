#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
extrn printf, putchar;
main(argc, argv) {
	auto i;
	i = 0;
	while(i < argc){
		printf("%s ", argv[i]);
		i = i + 1;
	}
	putchar(10);
	return(0);
}
)");
}
