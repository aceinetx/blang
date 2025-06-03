#include "blang.hh"
#include <parser.hh>

using namespace blang;

int main() {
  Blang blang = Blang("b");
  if (!blang.target)
    return 1;

  blang.input = R"(
base; word; n;

display() {
    extrn printf;
    auto it, i;

    it = base;
    i  = 0;
    while (i < n) {
        if (*it) printf("#"); else printf(".");
        it += word;
        i  += 1;
    }
    printf("\n");
}

next() {
    auto it, i, state;

    it = base;
    state = *it | *(it + word) << 1;
    i  = 2;
    it = base + 2*word;
    while (i < n) {
        state <<= 1;
        state  |= *it;
        state  &= 7;
        *(it - word) = (110>>state)&1;
        i  += 1;
        it += word;
    }
}

main() {
    extrn malloc, memset;

    n    = 100;
    word = 8;
    base = malloc(word*n);
    memset(base, 0, word*n);
    *(base + (n - 2)*word) = 1;

    display();
    auto i;
    i = 0;
    while (i < n - 3) {
        next();
        display();
        i += 1;
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
