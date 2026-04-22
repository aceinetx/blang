#include "Blang.hh"

using namespace blang;

int main(int argc, char **argv) {
  Blang blang = Blang("b");
  blang.compile("main() { auto x; auto y; x = y = 2; return (x + y); }");
}
