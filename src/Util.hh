#pragma once
#include <optional>
#include <string>

#define argsShift(...) blang::_argsShift(&argc, &argv)

namespace blang {
std::string _argsShift(int *argc, char ***argv);
std::optional<std::string> readFile(const std::string &filename);
} // namespace blang
