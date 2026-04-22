#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
main() {
	extrn printf;
	extrn putchar;
	auto i;
	i = 5;
	while(i > 0){
		while(0){ i = 0; }
		printf("%d", i);
		putchar(10);
		i = i - 1;
	}
	return(0);
})");
}
