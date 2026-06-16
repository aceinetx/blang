#pragma once
#include "fircpp/value.hpp"
#include <string>
#include <unordered_map>

namespace blang {
using Scope = std::unordered_map<std::string, fir::Value>;
} // namespace blang
