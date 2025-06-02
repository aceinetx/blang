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

  blang->builder.SetInsertPoint(i.falseBlock);
  for (AstNode *n : body) {
    if (!n->compile(blang))
      return false;
  }

  if (!blang->builder.GetInsertBlock()->getTerminator())
    blang->builder.CreateBr(i.mergeBlock);

  blang->builder.SetInsertPoint(i.mergeBlock);

  blang->ifs.pop();

  return true;
}
