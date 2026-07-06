#include "blangd/util.hh"
#include <cctype>
#include <stdexcept>

size_t blang::blangd::index_from_linecol(const std::string &text,
                                         size_t linePos, size_t colPos) {
  size_t currentLine = 0;
  size_t currentIndex = 0;

  while (currentLine < linePos && currentIndex < text.length()) {
    if (text[currentIndex] == '\n') {
      currentLine++;
    }
    currentIndex++;
  }

  if (currentLine < linePos) {
    throw std::out_of_range("Line position out of range");
  }

  size_t targetIndex = currentIndex + colPos;

  if (targetIndex > text.length()) {
    throw std::out_of_range("Column position out of range");
  }

  size_t checkIndex = currentIndex;
  while (checkIndex < targetIndex) {
    if (text[checkIndex] == '\n') {
      throw std::out_of_range("Column position out of range");
    }
    checkIndex++;
  }

  return targetIndex;
}

std::tuple<size_t, size_t> blang::blangd::word_range(const std::string &text,
                                                     size_t pos) {
  auto is_word = [&](size_t i) {
    return std::isalnum(static_cast<unsigned char>(text[i])) != 0 ||
           text[i] == '_';
  };

  if (text.empty())
    return {0, 0};

  if (pos >= text.size())
    pos = text.size() - 1;

  if (!is_word(pos))
    return {pos, pos};

  size_t l = pos;
  while (l > 0 && is_word(l - 1))
    --l;

  size_t r = pos;
  while (r + 1 < text.size() && is_word(r + 1))
    ++r;

  return {l, r};
}
