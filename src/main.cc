#include "blang.hh"

using namespace blang;

int main() {
  Blang blang = Blang("b");
  blang.input = R"(
main(argc){
	return 2 + 2 * 2;
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
