#include "blang.hh"
#include <parser.hh>

using namespace blang;

int main() {
  Blang blang = Blang("b");
  blang.input = R"(
main(argc){
	extrn puts;
	if(argc == 1){
		puts("argc == 1");
	} else if (argc == 2){
		puts("argc == 2");
	} else if (argc == 3){
		puts("argc == 3");
	} else {
		puts("none of the above");
	}

	return 0;
}
)";

  auto result = blang.parseAndCompile();
  if (result.is_error()) {
    fmt::println("blang: error: {}", result.get_error().value());
    return 1;
  }

  blang.emit("a.out", Blang::EMIT_IR);
}
