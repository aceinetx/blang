#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstElse::print(int indent) const {
  printIndent(indent);
  std::cout << "AstElse" << "\n";
  printIndent(indent + 1);
}

AstElse::~AstElse() {
  for (AstNode *n : body)
    delete n;
}

bool AstElse::compile(Blang *blang) {
  If &i = blang->ifs.top();

  blang->builder.SetInsertPoint(i.fallthrough);
  for (AstNode *n : body) {
    if (!n->compile(blang))
      return false;
  }

  blang->builder.CreateBr(i.end);
  blang->builder.SetInsertPoint(i.end);

  return true;
}
