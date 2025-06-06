#include "blang.hh"
#include "util.hh"
#include <parser.hh>

using namespace blang;

int main(int argc, char **argv) {
  std::string output = "a.out";
  Blang::EmitLevel emit_level = Blang::EMIT_EXE;

  Blang blang = Blang("b");
  if (!blang.target)
    return 1;

  argsShift();
  for ([[maybe_unused]] int i = 0; argc; ++i) {
    std::string arg = argsShift();

    if (arg.starts_with("-")) {
      if (arg == "-o") {
        output = argsShift();
      } else if (arg == "-c") {
        emit_level = Blang::EMIT_OBJ;
      } else if (arg == "-emit-llvm") {
        emit_level = Blang::EMIT_IR;
      } else if (arg == "-l") {
        blang.link_libraries.push_back(argsShift());
      } else if (arg == "-L") {
        blang.link_path = argsShift();
      } else if (arg == "-b") {
        blang.libb = true;
      } else if (arg == "--help") {
        fmt::printf(R"(OVERVIEW: blang LLVM compiler

USAGE: blang [options] file...

OPTIONS:
  --help              Print this message
  -o <file>           Write output to <file>
  -c                  Only run compile, and assemble steps
  -emit-llvm          Output LLVM IR
  -L <dir>            Add directory to library search path
  -l <lib>            Link libraries 
  -b                  Link with B runtime and B standard library
)");
        return 0;
      } else {
        fmt::printf("blang: error: unknown argument: {}", arg);
        return 1;
      }
    } else { /* assume it's a file */
      auto result = readFile(arg);
      if (result.is_error()) {
        fmt::println("blang: {}", result.get_error().value());
        return 1;
      }

      blang.input = result.get_success().value();
    }
  }

  auto result = blang.parseAndCompile();
  if (result.is_error()) {
    fmt::println("blang: {}", result.get_error().value());
    return 1;
  }

  result = blang.emit(output, emit_level);
  if (result.is_error()) {
    fmt::println("blang: {}", result.get_error().value());
    return 1;
  }
}
