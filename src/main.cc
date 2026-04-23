#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
main(argc, argv) {
	extrn strcmp;
	return(strcmp(*(argv+8), "hi") == 0);
}
)");
}
