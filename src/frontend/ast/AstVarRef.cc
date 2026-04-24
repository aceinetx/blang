#include "frontend/ast/AstVarRef.hh"
#include "Blang.hh"
#include "frontend/exceptions/UndeclaredNameException/UndeclaredNameException.hh"
#include <fmt/base.h>

namespace blang {
void AstVarRef::print(int indent) {
  printIndent(indent);
  fmt::println("- AstVarRef {}", name);
}

llvm::Value *AstVarRef::compile(Blang *blang, bool rvalue) {
  assert(name != "");

  auto *value = blang->get_scope_var(name);
  if (!value)
    throw UndeclaredNameException(location, name);
  return rvalue ? blang->builder.CreateLoad(blang->get_word_ty(), value)
                : blang->builder.CreatePtrToInt(value, blang->get_word_ty());
}
} // namespace blang
