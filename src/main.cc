#include "blang.hh"
#include <parser.hh>

using namespace blang;

int main() {
  Blang blang = Blang("b");
  blang.input = R"(
factorial(n){
	if(n < 2){
		return 1;
	}
	return n * factorial(n - 1);
}

main(argc){
	extrn printf, putchar;

	printf("%ld", factorial(6));
	putchar(10);

	return 0;
}
)";
  // 720

  auto result = blang.parseAndCompile();
  if (result.is_error()) {
    fmt::println("blang: error: {}", result.get_error().value());
    return 1;
  }

  blang.emit("a.out", Blang::EMIT_IR);
}
