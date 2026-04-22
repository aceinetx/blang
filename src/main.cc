#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
main() { 
	*0 = 1;
	return(0); 
}
)");
}
