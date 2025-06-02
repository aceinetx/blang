#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstElse::print(int indent) const {
  printIndent(indent);
  std::cout << "AstElse" << "\n";
  printIndent(indent + 1);
  body->print(indent + 2);
}

AstElse::~AstElse() {
  delete body;
}

bool AstElse::compile(Blang *blang) {
  If &i = blang->ifs.top();

  // Generate else block code in the false block
  blang->builder.SetInsertPoint(i.falseBlock);
  if (!doBlock(block))
    return false;

  // Add branch to merge block if not already terminated
  if (!blang->builder.GetInsertBlock()->getTerminator())
    blang->builder.CreateBr(i.mergeBlock);

  return true;
}
