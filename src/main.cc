#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
main(argc, argv) {
	return(2 + 2 * 2 == 3 + 3);
}
)");
}
