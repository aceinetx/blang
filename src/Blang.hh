#pragma once
#include "CompilerContext.hh"
#include <location.hh>

namespace blang {
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
  std::string source_filename = "<unknown>";

  void emit(std::string out);

private:
  CompilerContext context;
  char output[0x10000];
};
} // namespace blang
