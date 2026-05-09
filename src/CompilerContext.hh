#pragma once
#include "Scope.hh"
#include "frontend/ast/AstSwitch.hh"
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <location.hh>
#include <memory>

namespace blang {
struct CompilerContext {
  CompilerContext(std::string moduleName);

  llvm::Type *get_word_ty();

  void push_scope();

  void pop_scope();

  Scope &get_scope();

  llvm::Value *get_scope_var(std::string name);

  void add_scope_var(std::string name, llvm::Value *value,
                     std::optional<class location> diagnostic_location = {});

  void
  add_global_scope_var(std::string name, llvm::Value *value,
                       std::optional<class location> diagnostic_location = {});

  void update_global_scope_var(std::string name, llvm::Value *value);

  void setup_debug_info(const std::string &source_path);
  void finalize_debug_info();
  void set_debug_location(const class location &loc);
  void clear_debug_location();

  llvm::DIBasicType *get_word_di_type();

  void emit_local_var_debug_info(llvm::AllocaInst *alloca,
                                 const std::string &var_name,
                                 const class location &loc,
                                 bool is_param = false,
                                 unsigned param_index = 0);

  void emit_global_var_debug_info(llvm::GlobalVariable *gvar,
                                  const std::string &var_name,
                                  const class location &loc,
                                  llvm::DIType *type = nullptr);

  void emit_global_array_debug_info(llvm::GlobalVariable *inner_gvar,
                                    llvm::GlobalVariable *outer_gvar,
                                    const std::string &var_name,
                                    size_t element_count,
                                    const class location &loc);

  llvm::LLVMContext context;
  llvm::IRBuilder<> builder;
  llvm::Module fmodule;
  llvm::Function *current_function = nullptr;

  std::unordered_map<std::string, llvm::Value *> extern_values;
  std::unordered_map<std::string, llvm::BasicBlock *> goto_blocks;
  std::vector<std::pair<std::string, class location>> unresolved_goto_labels;
  llvm::BasicBlock *while_statement_end_block = nullptr;

  AstSwitch *last_switch = nullptr;

  std::vector<Scope> scopes;

  bool debug_enabled = false;
  std::unique_ptr<llvm::DIBuilder> di_builder;
  llvm::DICompileUnit *di_compile_unit = nullptr;
  llvm::DIFile *di_file = nullptr;
  llvm::DISubprogram *di_current_subprogram = nullptr;
  llvm::DIBasicType *di_word_type = nullptr;
};
} // namespace blang
