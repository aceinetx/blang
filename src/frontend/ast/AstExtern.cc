#include "frontend/ast/AstExtern.hh"
#include "Assert.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
AstExtern::AstExtern() : names(std::make_shared<AstIdentifierList>()) {
}

void AstExtern::print(int indent) {
  printIndent(indent);
  fmt::print("- AstExtern\n");
  names->print(indent + 1);
  fmt::print("\n");
}

fir::Value AstExtern::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  blangassert(0 && "Unsupported");
#if 0
  for (const auto &pair : names->identifiers) {
    const auto &name = pair.first;
    const auto &location = pair.second;

    // if we already have the symbol in this module just use it
    auto existing = C->get_scope_var(name);
    if (existing) {
      C->add_scope_var(name, *existing, location);
      continue;
    }

    if (!C->extern_values.contains(name)) {
      auto *func =
          new GlobalVariable(C->fmodule, C->get_word_ty(), false,
                             GlobalValue::ExternalLinkage, nullptr, name);
      auto *GV = new GlobalVariable(C->fmodule, C->get_word_ty(), false,
                                    GlobalValue::PrivateLinkage, func);
      C->extern_values[name] = GV;
      C->add_scope_var(name, GV, location);
    } else {
      C->add_scope_var(name, C->extern_values[name], location);
    }
  }
#endif

  return {0};
}
} // namespace blang
