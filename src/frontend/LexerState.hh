#pragma once
#include <cstddef>

struct LexerState {
  std::size_t pos = 0;
  int line = 0;
  int column = 0;
};
