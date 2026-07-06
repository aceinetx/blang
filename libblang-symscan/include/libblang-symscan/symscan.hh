#pragma once
#include "libblang-symscan/ScanResult.hh"
#include "location.hh"
#include <optional>
#include <tuple>

namespace blang::symscan {
ScanResult scan_source(const std::string &source,
                       std::optional<position> context_position = {});
} // namespace blang::symscan
