#pragma once
#include "libblang-symscan/ScanResult.hh"

namespace blang::symscan {
ScanResult scan_source(const std::string &source);
} // namespace blang::symscan
