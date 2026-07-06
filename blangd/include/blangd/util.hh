#pragma once
#include <cstddef>
#include <string>
#include <tuple>

namespace blang::blangd {
size_t index_from_linecol(const std::string &text, size_t linePos,
                          size_t colPos);
std::tuple<size_t, size_t> word_range(const std::string &text, size_t pos);
} // namespace blang::blangd
