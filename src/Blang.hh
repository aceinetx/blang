#pragma once
#include "Unit.hh"
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
struct Blang {
  Blang(std::filesystem::path build_directory);
  Blang(const Blang &) = delete;
  Blang(Blang &&) = delete;
  Blang &operator=(const Blang &) = delete;
  Blang &operator=(Blang &&) = delete;
  ~Blang();

  struct CompileOptions {
    bool autorun = false;
    bool clean = false;
    bool link = true;
    std::vector<std::string> link_libraries = {};
    std::vector<std::string> link_paths = {};
    std::string output_exe = "a.out";
  };

  void add_unit(std::string code, std::filesystem::path source_path, bool debug,
                bool print_ast, llvm::OptimizationLevel optimizationLevel);

  void compile(CompileOptions &opt);

private:
  std::vector<Unit> units;
  std::mutex compilation_lock;
  std::filesystem::path build_directory;
};
} // namespace blang
