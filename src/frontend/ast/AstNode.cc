#include "AstNode.hh"

namespace blang {
AstNode::~AstNode() = default;

void AstNode::print(int indent) {
}

void AstNode::printIndent(int indent) {
  for (int i = 0; i < indent; i++) {
    std::putchar('\t');
  }
}
} // namespace blang
