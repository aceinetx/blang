#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile(R"(
main(argc, argv) {
	extrn puts;
	goto a;
	while(argc > 0){
		if(argc == 1){
			puts("1");
		} else if(argc == 2){
			puts("2");
		} else if(argc == 3){
			puts("2");
		} else if(argc == 4){
			puts("4");
		} else {
			puts("other");
		}
		a:
		argc = argc - 1;
	}
	return(0);
}
)");
}
