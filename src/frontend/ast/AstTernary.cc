#include "frontend/ast/AstTernary.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstTernary::print(int indent) {
  printIndent(indent);
  fmt::print("- AstTernary\n");
  expression->print(indent + 1);
  true_expression->print(indent + 1);
  false_expression->print(indent + 1);
}

llvm::Value *AstTernary::compile(CompilerContext *C, bool rvalue) {
  return C->builder.CreateSelect(expression->compile(C, true),
                                 true_expression->compile(C, true),
                                 false_expression->compile(C, true));
}

} // namespace blang
