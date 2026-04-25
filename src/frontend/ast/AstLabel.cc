#include "frontend/ast/AstLabel.hh"
#include "Blang.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstLabel::print(int indent) {
  printIndent(indent);
  fmt::print("- AstLabel {}\n", name);
}

llvm::Value *AstLabel::compile(Blang *blang, bool rvalue) {
  (void)rvalue;

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

    for (ssize_t i = 0; i < (ssize_t)blang->unresolved_goto_labels.size();
         i++) {
      if (blang->unresolved_goto_labels[i].first == name) {
        blang->unresolved_goto_labels.erase(
            blang->unresolved_goto_labels.begin() + i);
      }
    }
  }
  return nullptr;
}

} // namespace blang
