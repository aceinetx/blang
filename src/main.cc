#include "Blang.hh"
#include "Util.hh"
#include <filesystem>
#include <fmt/core.h>
#include <getopt.h>

using namespace blang;

int main(int argc, char **argv) {
  bool debug = false;
  bool print_ast = false;
  auto optimizationLevel = llvm::OptimizationLevel::O1;
  std::vector<std::filesystem::path> sources;
  std::string build_dir = ".build";
  Blang::CompileOptions copt;

  argsShift();
  for ([[maybe_unused]] int i = 0; argc; ++i) {
    std::string arg = argsShift();

    if (arg.starts_with("-")) {
      if (arg == "-help") {
        fmt::print(R"(OVERVIEW: blang LLVM compiler

USAGE: blang [options] file...

OPTIONS:
  -help               Print this message
  -c                  Don't link final executable
  -o <file>           Write final executable to <file>
  -o-dir <path>       Set the build directory path
  -L <dir>            Add directory to library search path
  -l <lib>            Link libraries 
  -O(0|1|2|3|s|z)     Set optimization level
  -g                  Emit DWARF debug information
  -ast                Print ast
  -autorun            Automatically run the final executable (only takes effect without -c flag)
  -clean              Automatically remove the build directory (only takes effect with -autorun flag)
)");
        return 0;
      } else if (arg == "-c") {
        copt.link = false;
      } else if (arg == "-o") {
        copt.output_exe = argsShift();
      } else if (arg == "-o-dir") {
        build_dir = argsShift();
      } else if (arg == "-L") {
        copt.link_paths.push_back(argsShift());
      } else if (arg == "-l") {
        copt.link_libraries.push_back(argsShift());
      } else if (arg == "-O0") {
        optimizationLevel = llvm::OptimizationLevel::O0;
      } else if (arg == "-O1") {
        optimizationLevel = llvm::OptimizationLevel::O1;
      } else if (arg == "-O2") {
        optimizationLevel = llvm::OptimizationLevel::O2;
      } else if (arg == "-O3") {
        optimizationLevel = llvm::OptimizationLevel::O3;
      } else if (arg == "-Os") {
        optimizationLevel = llvm::OptimizationLevel::Os;
      } else if (arg == "-Oz") {
        optimizationLevel = llvm::OptimizationLevel::Oz;
      } else if (arg == "-g") {
        debug = true;
      } else if (arg == "-ast") {
        print_ast = true;
      } else if (arg == "-autorun") {
        copt.autorun = true;
      } else if (arg == "-clean") {
        copt.clean = true;
      } else {
        fmt::print("blang: error: unknown argument: {}\n", arg);
        return 1;
      }
    } else {
      sources.emplace_back(arg);
    }
  }

  Blang blang{build_dir};

  for (auto &path : sources) {
    auto result = readFile(path);
    if (!result) {
      fmt::print("blang: could not read file {}\n", path.string());
      return 1;
    }

    blang.add_unit(*result, path, debug, print_ast, optimizationLevel);
  }

  blang.compile(copt);
}
