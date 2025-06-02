#include "blang.hh"
#include <parser.hh>

using namespace blang;

int main() {
  Blang blang = Blang("b");
  blang.input = R"(
main(){
	auto i, pi, ppi;	
	pi = &i;
	ppi = &pi;
	**ppi = 69;

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
