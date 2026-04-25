#include "frontend/ast/AstStringLit.hh"
#include "Assert.hh"
#include "Blang.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstStringLit::print(int indent) {
  printIndent(indent);
  fmt::print("- AstStringLit {}\n", str);
}

llvm::Value *AstStringLit::compile(Blang *blang, bool rvalue) {
  blangassert(rvalue);
  return blang->builder.CreatePtrToInt(blang->builder.CreateGlobalString(str),
                                       blang->get_word_ty());
}

} // namespace blang
