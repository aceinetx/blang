#include "AstAutoVar.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstAutoVar::print(int indent) {
  printIndent(indent);
  fmt::print("- AstAutoVar\n");
  list->print(indent + 1);
}

fir::Value AstAutoVar::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  for (const auto &entry : *list) {
    auto value = C->ir.alloca(C->get_word_ty());
    fir::Value init;
    if (entry.initializer) {
      init = entry.initializer->compile(C, true);
    } else {
      init = C->ir.constant(C->get_word_ty(), 0L);
    }
    C->ir.store(value, init);
    C->add_scope_var(entry.identifier, value, entry.identifier_location);
  }
  return {0};
}

} // namespace blang
