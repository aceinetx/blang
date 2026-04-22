#include "frontend/ast/AstLabel.hh"
#include "Blang.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void AstLabel::print(int indent) {
  printIndent(indent);
  fmt::println("- AstLabel {}", name);
}

llvm::Value *AstLabel::compile(Blang *blang) {
  if (!blang->goto_blocks.contains(name)) {
    auto block =
        BasicBlock::Create(blang->context, name, blang->current_function);
    if (!blang->builder.GetInsertBlock()->getTerminator()) {
      blang->builder.CreateBr(block);
    }
    blang->builder.SetInsertPoint(block);
    blang->goto_blocks[name] = block;
  } else {
    if (!blang->builder.GetInsertBlock()->getTerminator()) {
      blang->builder.CreateBr(blang->goto_blocks[name]);
    }
    blang->builder.SetInsertPoint(blang->goto_blocks[name]);
  }
  return nullptr;
}

} // namespace blang
