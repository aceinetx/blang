#include "Blang.hh"
#include "Assert.hh"
#include "frontend/Driver.hh"

#include "parser.tab.hpp"
#include <cstdio>
#include <fmt/core.h>
#include <fstream>
#include <ostream>

namespace blang {
Blang::Blang(std::string moduleName) : context(moduleName) {
  context.push_scope();
}

Blang::~Blang() = default;

void Blang::compile(std::string code) {
  Driver driver = Driver(code);
  Parser parser = Parser(driver);
  parser.parse();
  driver.get_root()->compile(&context);

  FILE *fp = fmemopen(output, 0x10000 - 1, "w");
  blangassert(fp);

  context.ir.print(fp);

  fclose(fp);
}

void Blang::bindings(std::string code, std::ostream &stream) {
  Driver driver = Driver(code);
  Parser parser = Parser(driver);
  parser.parse();

  auto guard_name = "BLANG_FIR";
  stream << "#ifndef " << guard_name << "\n";
  stream << "#define " << guard_name << "\n";
  stream << "#include <stdint.h>\n";
  stream << "#include <stddef.h>\n";

  driver.get_root()->bindings(stream);

  stream << "#endif\n";
}

void Blang::emit(std::string filename) {
  std::ofstream f(filename);
  f << std::string(output);
  f.close();
}

} // namespace blang
