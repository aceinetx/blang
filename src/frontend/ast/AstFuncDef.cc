#include "frontend/ast/AstFuncDef.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/UnresolvedLabelException/UnresolvedLabelException.hh"
#include <array>
#include <fmt/core.h>
#include <unordered_map>

namespace blang {
AstFuncDef::AstFuncDef() : args(std::make_shared<AstIdentifierList>()) {
}

void AstFuncDef::print(int indent) {
  printIndent(indent);
  fmt::print("- AstFuncDef\n");
  args->print(indent + 1);
  node_block->print(indent + 1);
}

fir::Value AstFuncDef::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;

  std::array<fir::type::Type, FR_FUNC_MAX_ARGS> arg_types{};
  for (size_t i = 0; i < args->identifiers.size(); i++)
    arg_types[i] = C->get_word_ty();

  auto func = C->ir.func(C->get_word_ty(), name.c_str(), arg_types);

  C->push_scope();
  C->current_function = func;
  C->unresolved_goto_labels.clear();
  C->goto_blocks.clear();
  C->while_statement_end_block = nullptr;

  /* Initialize arguments */
  for (size_t i = 0; i < args->identifiers.size(); i++) {
    const auto &name = args->identifiers[i].first;
    const auto &location = args->identifiers[i].second;

    {
      auto var = C->ir.alloca(C->get_word_ty());
      auto arg = C->ir.arg(i);
      C->ir.store(var, arg);
      C->add_scope_var(name, var, location);
    }
  }

  /* Compile body */
  fir::Value last = node_block->compile(C, true);

  C->pop_scope();
  C->current_function = nullptr;

  /*
   * Check for unresolved goto labels
   */
  for (const auto &label : C->unresolved_goto_labels) {
    throw UnresolvedLabelException(label.second, label.first);
  }

  auto v = C->ir.constant(C->get_word_ty(), 0L);
  C->ir.ret(v);

  return last;
}

void AstFuncDef::bindings(std::ostream &os) {
  os << "extern int64_t " << name << " (int64_t arg, ...);\n";

  os << "#define " << name << "(";
  for (const auto &arg : args->identifiers) {
    if (&arg != &args->identifiers[0]) {
      os << ",";
    }
    os << arg.first;
  }
  os << ") " << name << "(";
  for (const auto &arg : args->identifiers) {
    if (&arg != &args->identifiers[0]) {
      os << ",";
    }
    os << "(int64_t)(" << arg.first << ")";
  }
  os << ")\n";
}

} // namespace blang
