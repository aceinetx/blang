#include "CompilerContext.hh"
#include "frontend/exceptions/RedefinitionException/RedefinitionException.hh"

namespace blang {
CompilerContext::CompilerContext(std::string moduleName)
    : context(), builder(context), fmodule(moduleName, context) {
}

llvm::Type *CompilerContext::get_word_ty() {
  llvm::DataLayout DL = fmodule.getDataLayout();
  unsigned maxIntSize = DL.getLargestLegalIntTypeSizeInBits();
  llvm::Type *maxIntType = llvm::IntegerType::get(context, maxIntSize);
  return maxIntType;
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

llvm::Value *CompilerContext::get_scope_var(std::string name) {
  for (auto &scope : scopes) {
    if (scope.contains(name))
      return scope[name];
  }
  return nullptr;
}

void CompilerContext::add_scope_var(
    std::string name, llvm::Value *value,
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
    std::string name, llvm::Value *value,
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
                                              llvm::Value *value) {
  scopes.back()[name] = value;
}
} // namespace blang
