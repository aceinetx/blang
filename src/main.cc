#include "blang.hh"
#include <parser.hh>

using namespace blang;

int main() {
  Blang blang = Blang("b");
  blang.input = R"(
main(argc){
	extrn printf;
	if(argc == 1){
		printf("argc == 1");
	} else if (argc == 2){
		printf("argc == 2");
	} else if (argc == 3){
		printf("argc == 3");
	} else {
		printf("none of the above");
	}

	return i;
}
)";

  auto result = blang.parseAndCompile();
  if (result.is_error()) {
    fmt::println("blang: error: {}", result.get_error().value());
    return 1;
  }

  blang.emit("a.out", Blang::EMIT_IR);
}
