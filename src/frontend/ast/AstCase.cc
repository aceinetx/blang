#include "frontend/ast/AstCase.hh"
#include "Assert.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstCase::print(int indent) {
  printIndent(indent);
  fmt::print("- AstCase {}\n", number);
}

llvm::Value *AstCase::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  blangassert(C->last_switch);
  C->set_debug_location(location);

  auto block = BasicBlock::Create(C->context, "", C->current_function);
  C->builder.CreateBr(block);

  C->last_switch->add_case(C, number, block);

  C->builder.SetInsertPoint(block);
  return nullptr;
}

} // namespace blang
