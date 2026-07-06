#include "Blang.hh"
#include "Util.hh"
#include "frontend/DiagnosticPrinter/DiagnosticPrinter.hh"
#include <fmt/core.h>
#include <fstream>

using namespace blang;

int main(int argc, char **argv) {
  std::string output = "a.out";
  EmitLevel emit_level = EmitLevel::EMIT_EXE;

  Blang blang = Blang("b");
  std::string input = "";

  bool bindings = false;
  std::string bindings_out = "";

  argsShift();
  for ([[maybe_unused]] int i = 0; argc; ++i) {
    std::string arg = argsShift();

    if (arg.starts_with("-")) {
      if (arg == "-o") {
        output = argsShift();
      } else if (arg == "-c") {
        emit_level = EmitLevel::EMIT_OBJ;
      } else if (arg == "-emit-llvm") {
        emit_level = EmitLevel::EMIT_IR;
      } else if (arg == "-l") {
        blang.link_libraries.push_back(argsShift());
      } else if (arg == "-L") {
        blang.link_paths.push_back(argsShift());
      } else if (arg == "--bindings") {
        bindings = true;
      } else if (arg == "--bindings-out") {
        bindings_out = argsShift();
      } else if (arg == "-O0") {
        blang.optimizationLevel = llvm::OptimizationLevel::O0;
      } else if (arg == "-O1") {
        blang.optimizationLevel = llvm::OptimizationLevel::O1;
      } else if (arg == "-O2") {
        blang.optimizationLevel = llvm::OptimizationLevel::O2;
      } else if (arg == "-O3") {
        blang.optimizationLevel = llvm::OptimizationLevel::O3;
      } else if (arg == "-Os") {
        blang.optimizationLevel = llvm::OptimizationLevel::Os;
      } else if (arg == "-Oz") {
        blang.optimizationLevel = llvm::OptimizationLevel::Oz;
      } else if (arg == "-g") {
        blang.debug = true;
      } else if (arg == "--ast") {
        blang.print_ast = true;
      } else if (arg == "--help") {
        fmt::print(R"(OVERVIEW: blang LLVM compiler

USAGE: blang [options] file...

OPTIONS:
  --help              Print this message
  -o <file>           Write output to <file>
  -c                  Only run compile, and assemble steps
  -emit-llvm          Output LLVM IR
  -L <dir>            Add directory to library search path
  -l <lib>            Link libraries 
  --bindings          Generate .h C bindings
  --bindings-out      Generate .h C bindings alongside compilation
  -O(0|1|2|3|s|z)     Set optimization level
  -g                  Emit DWARF debug information
)");
        return 0;
      } else {
        fmt::print("blang: error: unknown argument: {}\n", arg);
        return 1;
      }
    } else { /* assume it's a file */
      auto result = readFile(arg);
      if (!result) {
        fmt::print("blang: could not open file {}\n", arg);
        return 1;
      }

      input = *result;
      blang.source_filename = arg;
    }
  }

  DiagnosticPrinter diag_printer = DiagnosticPrinter("b", input);

  try {
    if (!bindings) {
      blang.compile(input);
      blang.emit(output, emit_level);

      if (bindings_out.empty()) {
        return 0;
      }

      output = bindings_out;
    }

    auto file = std::ofstream(output);
    if (!file.is_open()) {
      fmt::print("blang: could not open file {}\n", output);
      return 1;
    }
    blang.bindings(input, file);
    file.close();
    fmt::print("blang: bindings written to {}\n", output);
  } catch (LexerException &exc) {
    diag_printer.printDiagnostic(exc);
  } catch (ParserException &exc) {
    diag_printer.printDiagnostic(exc);
  } catch (LocationException &exc) {
    diag_printer.printDiagnostic(exc);
  }
}
