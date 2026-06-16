#include "CompilerContext.hh"
#include "frontend/exceptions/RedefinitionException/RedefinitionException.hh"

namespace blang {
CompilerContext::CompilerContext(std::string moduleName) : ir({}) {
}

CompilerContext::~CompilerContext() = default;

fir::type::Type CompilerContext::get_word_ty() {
  return &frIRType_u64;
}

fir::type::Type CompilerContext::get_word_ptr_ty() {
  static auto type =
      (frIRType){.unit = FR_TYPE_PTR, .v = {.ptr = {.pointee = get_word_ty()}}};
  return &type;
}

void CompilerContext::push_scope() {
  scopes.insert(scopes.begin(), Scope());
}

void CompilerContext::pop_scope() {
  scopes.erase(scopes.begin());
}

Scope &CompilerContext::get_scope() {
  return scopes[0];
}

std::optional<fir::Value> CompilerContext::get_scope_var(std::string name) {
  for (auto &scope : scopes) {
    if (scope.contains(name))
      return scope[name];
  }
  return {};
}

void CompilerContext::add_scope_var(
    std::string name, fir::Value value,
    std::optional<class location> diagnostic_location) {
  auto &scope = get_scope();
  if (scope.contains(name)) {
    if (diagnostic_location)
      throw RedefinitionException(*diagnostic_location, name);
    throw std::runtime_error("redefinition of " + name);
  }
  scope[name] = value;
}

void CompilerContext::add_global_scope_var(
    std::string name, fir::Value value,
    std::optional<class location> diagnostic_location) {
  auto &scope = scopes.back();
  if (scope.contains(name)) {
    if (diagnostic_location)
      throw RedefinitionException(*diagnostic_location, "global " + name);
    throw std::runtime_error("redefinition of global " + name);
  }
  scope[name] = value;
}

void CompilerContext::update_global_scope_var(std::string name,
                                              fir::Value value) {
  scopes.back()[name] = value;
}
} // namespace blang
