#include "frontend/ast/AstStringLit.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

namespace blang {
void AstStringLit::print(int indent) {
  printIndent(indent);
  fmt::print("- AstStringLit {}\n", str);
}

fir::Value AstStringLit::compile(CompilerContext *C, bool rvalue) {
  if (!rvalue) {
    throw LvalueException(location, "string literal");
  }
  auto var = C->ir.constant(str);
  return var;
}

} // namespace blang
