#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
main() { 
	auto x;
	auto y;
	x = 2 * (y = 2);
	return(x); 
}
)");
}
