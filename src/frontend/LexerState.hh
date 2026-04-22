#pragma once
#include <cstddef>

struct LexerState {
  std::size_t pos = 0;
  std::size_t line = 1;
  std::size_t line_start_pos = 0;
};
