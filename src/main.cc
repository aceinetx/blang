#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
main() {
	extrn putchar;
	
	goto a;

b:
	putchar('b');
	goto end;

a:
	putchar('a');
	goto b;

end:
	putchar('e');
	putchar(10);
	return(0);
})");
}
