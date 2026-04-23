#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
main(argc, argv) {
	extrn puts;
	auto i;
	i = 69;
	return(i);
}
)");
}
