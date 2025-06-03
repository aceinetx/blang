#include "util.hh"
#include <pch.hh>

std::string _argsShift(int *argc, char ***argv) {
  if (*argc <= 0) {
    return "";
  }

  --(*argc);
  return *(*argv)++;
}

Result<std::string, std::string> readFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file) {
    return Result<std::string, std::string>::error(
        fmt::format("could not open {}", filename));
    exit(1);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  return Result<std::string, std::string>::success(buffer.str());
}
