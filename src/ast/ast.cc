#include "ast.hh"
#include <pch.hh>

using namespace blang;

AstNode::~AstNode() {
  for (AstNode *node : children) {
    delete node;
  }
}

void AstNode::printIndent(int indent) const {
  for (int i = 0; i < indent; ++i) {
    std::cout << "  ";
  }
}

bool AstNode::compile(Blang *blang) {
  for (AstNode *node : children) {
    if (!node->compile(blang))
      return false;
  }
  return true;
}
