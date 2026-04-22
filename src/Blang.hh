#pragma once
#include "Scope.hh"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

namespace blang {
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

  void add_scope_var(std::string name, llvm::Value *value);

  void add_global_scope_var(std::string name, llvm::Value *value);

  llvm::LLVMContext context;
  llvm::IRBuilder<> builder;
  llvm::Module fmodule;
  llvm::Function *current_function;

  std::unordered_map<std::string, llvm::Value *> extern_values;

private:
  std::string targetTriple;
  const llvm::Target *target;
  std::unique_ptr<llvm::TargetMachine> targetMachine;

  std::vector<Scope> scopes;
};
} // namespace blang
