#include "frontend/ast/AstStringLit.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/LvalueException/LvalueException.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstStringLit::print(int indent) {
  printIndent(indent);
  fmt::print("- AstStringLit {}\n", str);
}

llvm::Value *AstStringLit::compile(CompilerContext *C, bool rvalue) {
  if (!rvalue) {
    throw LvalueException(location, "string literal");
  }
  Constant *c = ConstantDataArray::getString(C->context, str);
  auto *var = new GlobalVariable(C->fmodule, c->getType(), true,
                                 GlobalValue::ExternalLinkage, c);
  return var;
}

} // namespace blang
