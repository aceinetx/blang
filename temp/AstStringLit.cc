#include "frontend/ast/AstStringLit.hh"
#include "Blang.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void AstStringLit::print(int indent) {
  printIndent(indent);
  fmt::println("- AstStringLit {}", str);
}

llvm::Value *AstStringLit::compile(Blang *blang) {
  return blang->builder.CreatePtrToInt(blang->builder.CreateGlobalString(str),
                                       blang->get_word_ty());
}

} // namespace blang
