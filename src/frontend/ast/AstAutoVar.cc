#include "AstAutoVar.hh"
#include "Blang.hh"
#include <fmt/base.h>

namespace blang {
void AstAutoVar::print(int indent) {
  printIndent(indent);
  fmt::println("- AstAutoVar {}", name);
}

llvm::Value *AstAutoVar::compile(Blang *blang) {
  assert(name != "");

  return nullptr;
}

} // namespace blang
