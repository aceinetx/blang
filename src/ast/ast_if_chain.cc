#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstIfChain::print(int indent) const {
  printIndent(indent);
  std::cout << "AstIfChain" << "\n";
  printIndent(indent + 1);
}

AstIfChain::~AstIfChain() {
  for (AstNode *n : ifs)
    delete n;
}

bool AstIfChain::compile(Blang *blang) {
  for (AstNode *n : ifs) {
    if (!n->compile(blang)) {
      return false;
    }
  }

  if (!blang->ifs.top().false_block->getTerminator()) {
    blang->builder.SetInsertPoint(blang->ifs.top().false_block);
    blang->builder.CreateBr(blang->ifs.top().merge_block);
  }

  blang->builder.SetInsertPoint(blang->ifs.top().merge_block);
  blang->ifs.pop();

  return true;
}
