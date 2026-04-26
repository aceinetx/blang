#include "frontend/ast/AstLabel.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstLabel::print(int indent) {
  printIndent(indent);
  fmt::print("- AstLabel {}\n", name);
}

llvm::Value *AstLabel::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;

  if (!C->goto_blocks.contains(name)) {
    auto block =
        BasicBlock::Create(C->context, name, C->current_function);
    if (!C->builder.GetInsertBlock()->getTerminator()) {
      C->builder.CreateBr(block);
    }
    C->builder.SetInsertPoint(block);
    C->goto_blocks[name] = block;
  } else {
    if (!C->builder.GetInsertBlock()->getTerminator()) {
      C->builder.CreateBr(C->goto_blocks[name]);
    }
    C->builder.SetInsertPoint(C->goto_blocks[name]);

    for (ssize_t i = 0; i < (ssize_t)C->unresolved_goto_labels.size();
         i++) {
      if (C->unresolved_goto_labels[i].first == name) {
        C->unresolved_goto_labels.erase(
            C->unresolved_goto_labels.begin() + i);
      }
    }
  }
  return nullptr;
}

} // namespace blang
