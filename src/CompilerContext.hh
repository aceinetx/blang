#pragma once
#include "Scope.hh"
#include "frontend/ast/AstSwitch.hh"
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
};
} // namespace blang
