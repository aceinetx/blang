#include "Blang.hh"
#include "Assert.hh"
#include <filesystem>
#include <fmt/core.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>
#include <thread>
#include <utility>

using namespace llvm;

static void run_command(const std::string &cmd) {
  fmt::print("blang: running command: {}\n", cmd);
  std::system(cmd.c_str());
}

namespace blang {
Blang::Blang(std::filesystem::path build_directory)
    : build_directory(std::move(build_directory)) {
  InitializeNativeTarget();
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();
}

Blang::~Blang() = default;

void Blang::add_unit(std::string code, std::filesystem::path source_path,
                     bool debug, bool print_ast,
                     llvm::OptimizationLevel optimizationLevel) {
  units.emplace_back(code, source_path, debug, print_ast, optimizationLevel,
                     build_directory);
}

void Blang::compile(CompileOptions &opt) {
  blangassert(!units.empty());
  std::lock_guard<std::mutex> _guard(compilation_lock);

  std::filesystem::create_directory(build_directory);

  std::vector<std::jthread> threads;
  std::atomic<std::size_t> success_count;

  for (auto &unit : units) {
    std::jthread thread(&Unit::compile, &unit, &success_count);
    threads.emplace_back(std::move(thread));
  }

  for (auto &thread : threads) {
    thread.join();
  }

  if (success_count != units.size()) {
    fmt::print("blang: error: not all units compiled\n");
    goto exit;
  }

  if (opt.link) {
    /* Link all of em object files */

    std::string objects;
    for (const auto &entry :
         std::filesystem::directory_iterator(build_directory)) {
      auto path = entry.path();
      if (path.extension() == ".o") {
        objects += path.string() + " ";
      }
    }

    std::string command = "cc -o " + opt.output_exe + " " + objects;
    for (const auto &path : opt.link_paths) {
      command += "-L" + path;
    }

    for (const auto &lib : opt.link_libraries) {
      command += "-l" + lib;
    }

    run_command(command);

    if (opt.autorun) {
      auto path = std::filesystem::absolute(opt.output_exe);
      run_command(path);

      if (opt.clean) {
        fmt::print("blang: cleanup\n");
        std::filesystem::remove(path);
        std::filesystem::remove_all(build_directory);
      }
    }
  }

exit:
  units.clear();
}
} // namespace blang
