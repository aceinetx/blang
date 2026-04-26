#include "frontend/ast/AstVarRef.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/UndeclaredNameException/UndeclaredNameException.hh"
#include <fmt/core.h>

namespace blang {
void AstVarRef::print(int indent) {
  printIndent(indent);
  fmt::print("- AstVarRef {}\n", name);
}

llvm::Value *AstVarRef::compile(CompilerContext *C, bool rvalue) {
  assert(name != "");

  auto *value = C->get_scope_var(name);
  if (!value)
    throw UndeclaredNameException(location, name);
  return rvalue ? C->builder.CreateLoad(C->get_word_ty(), value)
                : C->builder.CreatePtrToInt(value, C->get_word_ty());
}
} // namespace blang
