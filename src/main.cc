#include "blang.hh"
#include <parser.hh>

using namespace blang;

int main() {
  Blang blang = Blang("b");
  if (!blang.target)
    return 1;

  blang.input = R"(
main() {
    extrn printf;
    auto a, b, c;
    a = 0;
    b = 1;
    while (a < 1000000) {
        printf("%d\n", a);
        c = a + b;
        a = b;
        b = c;
    }
}
)";

  auto result = blang.parseAndCompile();
  if (result.is_error()) {
    fmt::println("blang: {}", result.get_error().value());
    return 1;
  }

  blang.emit("a.out", Blang::EMIT_IR);
}
