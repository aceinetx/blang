#include "AstAutoVar.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>
#include <llvm/IR/Instructions.h>

namespace blang {
void AstAutoVar::print(int indent) {
  printIndent(indent);
  fmt::print("- AstAutoVar\n");
  list->print(indent + 1);
}

llvm::Value *AstAutoVar::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  C->set_debug_location(location);
  for (const auto &entry : *list) {
    auto *value =
        C->builder.CreateAlloca(C->get_word_ty(), nullptr, entry.identifier);
    if (entry.initializer)
      C->builder.CreateStore(entry.initializer->compile(C, true), value);
    else
      C->builder.CreateStore(llvm::ConstantInt::get(C->get_word_ty(), 0),
                             value);
    C->add_scope_var(entry.identifier, value, entry.identifier_location);

    C->emit_local_var_debug_info(value, entry.identifier,
                                 entry.identifier_location);
  }

  return nullptr;
}

} // namespace blang
