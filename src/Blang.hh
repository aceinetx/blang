#pragma once
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

  llvm::Type *getWordTy();
  void compile(std::string code);

  std::string targetTriple;
  const llvm::Target *target;
  std::unique_ptr<llvm::TargetMachine> targetMachine;

  llvm::LLVMContext context;
  llvm::IRBuilder<> builder;
  llvm::Module fmodule;
};
} // namespace blang
