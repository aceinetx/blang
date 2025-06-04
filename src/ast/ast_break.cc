#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;

void AstBreak::print(int indent) const {
  printIndent(indent);
  std::cout << "AstBreak" << std::endl;
}

bool AstBreak::compile(Blang *blang) {
  if (blang->whiles.empty()) {
    blang->compile_error = "break outside of a while loop";
    return false;
  }

  blang->builder.CreateBr(blang->whiles.top().end);

  return true;
}
