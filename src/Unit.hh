#pragma once
#include "CompilerContext.hh"
#include <filesystem>

namespace blang {
struct Unit {
  Unit(std::string code, std::filesystem::path source_path, bool debug,
       bool print_ast, llvm::OptimizationLevel optimizationLevel,
       std::filesystem::path build_directory);
  Unit(const Unit &) = delete;
  Unit(Unit &&other) noexcept;
  Unit &operator=(const Unit &) = delete;
  Unit &operator=(Unit &&other) noexcept;
  ~Unit();

  void compile(std::atomic<std::size_t> *success_count);

  CompilerContext *context;
  std::string code;

private:
  void emit_object();
  void emit_ir();
  void emit_bindings();

  void compile_uncaught();

  bool debug;
  bool print_ast;
  std::string source_filename;
  std::filesystem::path build_directory;

  const llvm::Target *target;
  std::unique_ptr<llvm::TargetMachine> targetMachine;
  llvm::OptimizationLevel optimizationLevel;
};
} // namespace blang
