#include "blang.hh"
#include <parser.hh>

using namespace blang;

int main() {
  Blang blang = Blang("b");
  blang.input = R"(
main() {
	extrn malloc, free, printf, putchar;
	auto arr;
	arr = malloc(8*5);

	arr[0] = 68;
	printf("%d", arr[0] == 69);
	putchar(10);

	free(arr);
}
)";

  auto result = blang.parseAndCompile();
  if (result.is_error()) {
    fmt::println("[blang] {}", result.get_error().value());
    return 1;
  }

  blang.emit("a.out", Blang::EMIT_IR);
}
