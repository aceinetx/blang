#include "frontend/ast/AstExtern.hh"
#include "Blang.hh"
#include <fmt/base.h>
#include <llvm/IR/GlobalVariable.h>

using namespace llvm;

namespace blang {
void AstExtern::print(int indent) {
  printIndent(indent);
  fmt::print("- AstExtern ");
  for (const auto &name : names) {
    fmt::print("{} ", name);
  }
  fmt::println("");
}

llvm::Value *AstExtern::compile(Blang *blang) {
  for (const auto &name : names) {
    if (!blang->extern_values.contains(name)) {
      auto *GV =
          new GlobalVariable(blang->fmodule, blang->get_word_ty(), false,
                             GlobalValue::ExternalLinkage, nullptr, name);
      blang->extern_values[name] = GV;
      blang->add_scope_var(name, GV);
    } else {
      blang->add_scope_var(name, blang->extern_values[name]);
    }
  }

  return nullptr;
}
} // namespace blang
