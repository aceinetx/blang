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

namespace strings {
std::string replace_all(const std::string &str, const std::string &from,
                        const std::string &to) {
  if (from.empty())
    return str;

  std::string result = str;
  size_t start_pos = 0;
  while ((start_pos = result.find(from, start_pos)) != std::string::npos) {
    result.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return result;
}
std::vector<std::string> split(std::string s, const std::string &delimiter) {
  std::vector<std::string> tokens;
  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    token = s.substr(0, pos);
    tokens.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  tokens.push_back(s);

  return tokens;
}
} // namespace strings
} // namespace blang
