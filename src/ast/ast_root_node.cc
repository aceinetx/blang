#include "ast.hh"

using namespace blang;

void AstRootNode::print(int indent) const {
  printIndent(indent);
  std::cout << "AstRootNode" << std::endl;
  for (const auto &child : children) {
    child->print(indent + 1);
  }
}
