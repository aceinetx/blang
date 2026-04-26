#include "frontend/ast/AstStringLit.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstStringLit::print(int indent) {
  printIndent(indent);
  fmt::print("- AstStringLit {}\n", str);
}

llvm::Value *AstStringLit::compile(CompilerContext *C, bool rvalue) {
  if (!rvalue) {
    throw LvalueException(location, "string literal");
  }

  return C->builder.CreatePtrToInt(C->builder.CreateGlobalString(str),
                                       C->get_word_ty());
}

} // namespace blang
