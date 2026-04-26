#include "frontend/ast/AstGlobalVar.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>

using namespace llvm;

namespace blang {
void AstGlobalVar::print(int indent) {
  printIndent(indent);
  fmt::println("- AstGlobalVar");
  names->print(indent + 1);
}

llvm::Value *AstGlobalVar::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;

  for (const auto &pair : names->identifiers) {
    const auto &name = pair.first;
    const auto &location = pair.second;

    auto var =
        new GlobalVariable(C->fmodule, C->get_word_ty(), false,
                           llvm::GlobalValue::ExternalLinkage,
                           ConstantInt::get(C->get_word_ty(), 0), name);

    C->add_global_scope_var(name, var, location);
  }
  return nullptr;
}

} // namespace blang
