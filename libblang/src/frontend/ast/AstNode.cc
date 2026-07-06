#include "AstNode.hh"
#include <typeinfo>

namespace blang {
AstNode::~AstNode() = default;

void AstNode::print(int indent) {
}

void AstNode::printIndent(int indent) {
  for (int i = 0; i < indent; i++) {
    std::putchar('\t');
  }
}

void AstNode::bindings(std::ostream &os) {
  os << "// WARN: " << typeid(*this).name()
     << " doesn't have bindings overriden\n";
}
} // namespace blang
