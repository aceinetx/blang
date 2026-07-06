#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace blang::symscan {
struct FunctionScanResult {
  std::vector<std::string> args;
  std::vector<std::string> extrnSymbols;
  std::vector<std::string> autoSymbols;
};

struct ScanResult {
  std::string focusedFunction;
  std::vector<std::string> global;
  std::unordered_map<std::string, FunctionScanResult> functions;
};
} // namespace blang::symscan
