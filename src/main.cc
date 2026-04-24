#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
main(argc, argv) {
	auto x;
	x = 0;
	auto px;
	px = &x;
	px[0] = &0[1];
	return(px[0]);
}
)");
}
