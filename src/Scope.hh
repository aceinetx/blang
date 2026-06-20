#pragma once
#include <string>
#include <unordered_map>

namespace llvm {
class Value;
}

namespace blang {
using Scope = std::unordered_map<std::string, llvm::Value *>;
} // namespace blang
