#pragma once

namespace blang {
class Blang {
public:
  Blang();
  Blang(const Blang &) = default;
  Blang(Blang &&) = delete;
  Blang &operator=(const Blang &) = default;
  Blang &operator=(Blang &&) = delete;
  ~Blang();
};
} // namespace blang
