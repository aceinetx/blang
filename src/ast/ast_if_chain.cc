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
  auto id = ++blang->ifID;

  blang->ifs.push({});
  If &i = blang->ifs.top();
  i.fallthrough = BasicBlock::Create(blang->context, fmt::format("cmp{}", id),
                                     blang->current_func);
  i.end = BasicBlock::Create(blang->context, fmt::format("end{}", id),
                             blang->current_func);

  blang->builder.CreateBr(i.fallthrough);

  blang->builder.SetInsertPoint(i.fallthrough);

  for (AstNode *n : ifs) {
    if (!n->compile(blang)) {
      return false;
    }
  }

  if (!i.fallthrough->getTerminator()) {
    blang->builder.SetInsertPoint(i.fallthrough);
    blang->builder.CreateBr(i.end);
    blang->builder.SetInsertPoint(i.end);
  }

  blang->ifs.pop();

  return true;
}
