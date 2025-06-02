#include "blang.hh"
#include <parser.hh>

using namespace blang;

int main() {
  Blang blang = Blang("b");
  blang.input = R"(
main(argc, argv) {
    extrn printf, fprintf, stderr, atoi;

    auto W;
    W = 8;

    if (argc <= 1) {
        fprintf(stderr, "%s: missing operand\n", *argv);
        return(1);
    }

    auto i, n;
    n = atoi(*(argv + W));
    i = 1;
    while (i <= n) {
        printf("%d\n", i);
        i = i + 1;
    }
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
