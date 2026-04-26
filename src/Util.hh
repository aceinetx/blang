#pragma once
#include <optional>
#include <string>
#include <vector>

#define argsShift(...) blang::_argsShift(&argc, &argv)

namespace blang {
std::string _argsShift(int *argc, char ***argv);
std::optional<std::string> readFile(const std::string &filename);

namespace strings {
std::string replace_all(const std::string &str, const std::string &from,
                        const std::string &to);

std::vector<std::string> split(std::string s, const std::string &delimiter);
std::string remove_whitespace(const std::string &s);
} // namespace strings
} // namespace blang
