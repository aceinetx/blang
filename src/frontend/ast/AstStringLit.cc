#include "frontend/ast/AstStringLit.hh"
#include "Blang.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstStringLit::print(int indent) {
  printIndent(indent);
  fmt::print("- AstStringLit {}\n", str);
}

llvm::Value *AstStringLit::compile(Blang *blang, bool rvalue) {
  if (!rvalue) {
    throw LvalueException(location, "string literal");
  }

  return blang->builder.CreatePtrToInt(blang->builder.CreateGlobalString(str),
                                       blang->get_word_ty());
}

} // namespace blang
