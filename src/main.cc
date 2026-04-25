#include "Blang.hh"
#include "Util.hh"
#include "frontend/DiagnosticPrinter/DiagnosticPrinter.hh"
#include <fmt/core.h>

using namespace blang;

int main(int argc, char **argv) {
  std::string output = "a.out";
  EmitLevel emit_level = EmitLevel::EMIT_EXE;

  Blang blang = Blang("b");
  std::string input = "";

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
    }
  }

  DiagnosticPrinter diag_printer = DiagnosticPrinter("b", input);

  try {
    blang.compile(input);
    blang.emit(output, emit_level);
  } catch (LexerException &exc) {
    diag_printer.printDiagnostic(exc);
  } catch (ParserException &exc) {
    diag_printer.printDiagnostic(exc);
  } catch (LocationException &exc) {
    diag_printer.printDiagnostic(exc);
  }
}
