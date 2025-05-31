#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstExtrn::print(int indent) const {
  printIndent(indent);
  std::cout << "AstExtrn" << std::endl;
  printIndent(indent + 1);
  std::cout << "extrn ";
  for (auto name : names) {
    std::cout << name << " ";
  }
  std::cout << "\n";
}

bool AstExtrn::compile(Blang *blang) {
  for (auto name : names) {
    ExternSymbol ext;
    ext.name = name;
    ext.isExterned = false;
    ext.isFunction = true; // assume

    blang->extern_symbols[name] = ext;
  }

  return true;
}
