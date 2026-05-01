#pragma once
#include "CompilerContext.hh"
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

  void compile(std::string code);
  void bindings(std::string code, std::ostream &stream);

  std::vector<std::string> link_libraries = {};
  std::vector<std::string> link_paths = {};
  llvm::OptimizationLevel optimizationLevel = llvm::OptimizationLevel::O2;

  void emit(std::string out, EmitLevel level);

private:
  std::string targetTriple;
  const llvm::Target *target;
  std::unique_ptr<llvm::TargetMachine> targetMachine;

  CompilerContext context;
};
} // namespace blang
