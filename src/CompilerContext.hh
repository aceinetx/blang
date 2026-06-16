#pragma once
#include "Scope.hh"
#include "fircpp/ir.hpp"
#include "frontend/ast/AstSwitch.hh"
#include <location.hh>
#include <optional>
#include <vector>

namespace blang {
struct CompilerContext {
  CompilerContext(std::string moduleName);
  ~CompilerContext();

  fir::type::Type get_word_ty();
  fir::type::Type get_word_ptr_ty();

  void push_scope();

  void pop_scope();

  Scope &get_scope();

  std::optional<fir::Value> get_scope_var(std::string name);

  void add_scope_var(std::string name, fir::Value value,
                     std::optional<class location> diagnostic_location = {});

  void
  add_global_scope_var(std::string name, fir::Value value,
                       std::optional<class location> diagnostic_location = {});

  void update_global_scope_var(std::string name, fir::Value value);

  fir::IR ir;
  fir::Func current_function = nullptr;

  std::unordered_map<std::string, fir::Value> extern_values;
  std::unordered_map<std::string, fir::Block> goto_blocks;
  std::vector<std::pair<std::string, class location>> unresolved_goto_labels;
  fir::Block while_statement_end_block = nullptr;

  AstSwitch *last_switch = nullptr;

  std::vector<Scope> scopes;
};
} // namespace blang
