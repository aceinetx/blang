#include "frontend/ast/AstVarRef.hh"
#include "Assert.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/UndeclaredNameException/UndeclaredNameException.hh"
#include <fmt/core.h>

namespace blang {
void AstVarRef::print(int indent) {
  printIndent(indent);
  fmt::print("- AstVarRef {}\n", name);
}

fir::Value AstVarRef::compile(CompilerContext *C, bool rvalue) {
  blangassert(name != "");

  auto value_opt = C->get_scope_var(name);
  if (!value_opt)
    throw UndeclaredNameException(location, name);
  auto value = *value_opt;
  if (rvalue) {
    return C->ir.load(value, C->get_word_ty());
  } else {
    return C->ir.cast(value, C->get_word_ty());
  }
}
} // namespace blang
