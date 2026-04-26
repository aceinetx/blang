#include "frontend/ast/AstCase.hh"
#include "Assert.hh"
#include "Blang.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstCase::print(int indent) {
  printIndent(indent);
  fmt::println("- AstCase {}", number);
}

llvm::Value *AstCase::compile(Blang *blang, bool rvalue) {
  (void)rvalue;
  blangassert(blang->last_switch);

  auto block = BasicBlock::Create(blang->context, "", blang->current_function);
  blang->builder.CreateBr(block);

  blang->last_switch->add_case(blang, number, block);

  blang->builder.SetInsertPoint(block);
  return nullptr;
}

} // namespace blang
