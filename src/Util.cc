#include "Util.hh"
#include <fstream>
#include <sstream>

namespace blang {
std::string _argsShift(int *argc, char ***argv) {
  if (*argc <= 0) {
    return "";
  }

  --(*argc);
  return *(*argv)++;
}

std::optional<std::string> readFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file) {
    return {};
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  return buffer.str();
}
} // namespace blang
