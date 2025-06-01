#include "blang.hh"
#include <parser.hh>

using namespace blang;

int main() {
  Blang blang = Blang("b");
  blang.input = R"(
main(){

	return 0;
}
)";

  auto result = blang.parseAndCompile();
  if (result.is_error()) {
    fmt::println("[blang] {}", result.get_error().value());
    return 1;
  }

  blang.emit("a.out", Blang::EMIT_IR);
}
