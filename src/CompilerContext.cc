#include "CompilerContext.hh"
#include "frontend/exceptions/RedefinitionException/RedefinitionException.hh"
#include <filesystem>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DebugLoc.h>

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

void CompilerContext::setup_debug_info(const std::string &source_path) {
  debug_enabled = true;
  di_builder = std::make_unique<llvm::DIBuilder>(fmodule);

  std::filesystem::path p(source_path);
  std::string filename = p.filename().string();
  std::string directory =
      p.parent_path().empty() ? "." : p.parent_path().string();

  di_file = di_builder->createFile(filename, directory);

  di_compile_unit = di_builder->createCompileUnit(
      llvm::dwarf::DW_LANG_lo_user, di_file, "blang", false, "", 0);

  // mark debug info
  fmodule.addModuleFlag(llvm::Module::Warning, "Debug Info Version",
                        llvm::DEBUG_METADATA_VERSION);
  fmodule.addModuleFlag(llvm::Module::Warning, "Dwarf Version", 4);
}

void CompilerContext::finalize_debug_info() {
  if (!debug_enabled || !di_builder)
    return;
  di_builder->finalize();
}

void CompilerContext::set_debug_location(const class location &loc) {
  if (!debug_enabled || !di_current_subprogram)
    return;

  auto line = static_cast<unsigned>(loc.begin.line);
  auto col = static_cast<unsigned>(loc.begin.column);

  auto *scope = di_current_subprogram;
  builder.SetCurrentDebugLocation(
      llvm::DILocation::get(context, line, col, scope));
}

void CompilerContext::clear_debug_location() {
  if (!debug_enabled)
    return;
  builder.SetCurrentDebugLocation(llvm::DebugLoc());
}

llvm::DIBasicType *CompilerContext::get_word_di_type() {
  if (!di_word_type) {
    unsigned bits = get_word_ty()->getIntegerBitWidth();
    di_word_type =
        di_builder->createBasicType("word", bits, llvm::dwarf::DW_ATE_signed);
  }
  return di_word_type;
}

void CompilerContext::emit_local_var_debug_info(llvm::AllocaInst *alloca,
                                                const std::string &var_name,
                                                const class location &loc,
                                                bool is_param,
                                                unsigned param_index) {
  if (!debug_enabled || !di_builder || !di_current_subprogram)
    return;

  auto line = static_cast<unsigned>(loc.begin.line);
  auto *di_type = get_word_di_type();

  llvm::DILocalVariable *di_var = nullptr;
  if (is_param) {
    di_var = di_builder->createParameterVariable(di_current_subprogram,
                                                 var_name, param_index, di_file,
                                                 line, di_type, true);
  } else {
    di_var = di_builder->createAutoVariable(di_current_subprogram, var_name,
                                            di_file, line, di_type, true);
  }

  auto *di_expr = di_builder->createExpression();
  auto *di_loc = llvm::DILocation::get(context, line,
                                       static_cast<unsigned>(loc.begin.column),
                                       di_current_subprogram);

  llvm::Instruction *insert_before = alloca->getNextNode();
  if (insert_before) {
    di_builder->insertDeclare(alloca, di_var, di_expr, di_loc, insert_before);
  } else {
    di_builder->insertDeclare(alloca, di_var, di_expr, di_loc,
                              alloca->getParent());
  }
}

void CompilerContext::emit_global_var_debug_info(llvm::GlobalVariable *gvar,
                                                 const std::string &var_name,
                                                 const class location &loc,
                                                 llvm::DIType *type) {
  if (!debug_enabled || !di_builder || !di_compile_unit)
    return;

  auto line = static_cast<unsigned>(loc.begin.line);
  auto *di_type = type ? type : static_cast<llvm::DIType *>(get_word_di_type());

  auto *di_gve = di_builder->createGlobalVariableExpression(
      di_compile_unit, var_name, var_name, di_file, line, di_type, false);

  gvar->addDebugInfo(di_gve);
}

void CompilerContext::emit_global_array_debug_info(
    llvm::GlobalVariable *inner_gvar, llvm::GlobalVariable *outer_gvar,
    const std::string &var_name, size_t element_count,
    const class location &loc) {
  if (!debug_enabled || !di_builder || !di_compile_unit)
    return;

  auto line = static_cast<unsigned>(loc.begin.line);
  auto *elem_type = get_word_di_type();

  llvm::DINodeArray subscripts =
      di_builder->getOrCreateArray({di_builder->getOrCreateSubrange(
          0, static_cast<int64_t>(element_count))});
  auto *array_di_type = di_builder->createArrayType(
      element_count * elem_type->getSizeInBits(), 0, elem_type, subscripts);

  auto *inner_gve = di_builder->createGlobalVariableExpression(
      di_compile_unit, var_name + "__data", "" /* anonymous */, di_file, line,
      array_di_type, true);
  inner_gvar->addDebugInfo(inner_gve);

  auto *ptr_di_type = di_builder->createPointerType(
      elem_type, fmodule.getDataLayout().getPointerSizeInBits());

  auto *outer_gve = di_builder->createGlobalVariableExpression(
      di_compile_unit, var_name, var_name, di_file, line, ptr_di_type, false);
  outer_gvar->addDebugInfo(outer_gve);
}

} // namespace blang
