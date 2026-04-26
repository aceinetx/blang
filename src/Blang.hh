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
#include <location.hh>

namespace blang {
enum class EmitLevel {
  EMIT_EXE,
  EMIT_OBJ,
  EMIT_IR,
};

struct Blang {
  Blang(std::string moduleName);
  Blang(const Blang &) = delete;
  Blang(Blang &&) = delete;
  Blang &operator=(const Blang &) = delete;
  Blang &operator=(Blang &&) = delete;
  ~Blang();

  llvm::Type *get_word_ty();
  void compile(std::string code);

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

  std::vector<std::string> link_libraries = {};
  std::vector<std::string> link_paths = {};
  llvm::OptimizationLevel optimizationLevel = llvm::OptimizationLevel::O2;

  void emit(std::string out, EmitLevel level);

  llvm::LLVMContext context;
  llvm::IRBuilder<> builder;
  llvm::Module fmodule;
  llvm::Function *current_function;

  std::unordered_map<std::string, llvm::Value *> extern_values;
  std::unordered_map<std::string, llvm::BasicBlock *> goto_blocks;
  std::vector<std::pair<std::string, class location>> unresolved_goto_labels;
  llvm::BasicBlock *while_statement_end_block;

  AstSwitch *last_switch = nullptr;

private:
  std::string targetTriple;
  const llvm::Target *target;
  std::unique_ptr<llvm::TargetMachine> targetMachine;

  std::vector<Scope> scopes;
};
} // namespace blang
