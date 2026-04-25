#include "frontend/ast/AstGoto.hh"
#include "Blang.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstGoto::print(int indent) {
  printIndent(indent);
  fmt::println("- AstGoto {}", name);
}

llvm::Value *AstGoto::compile(Blang *blang, bool rvalue) {
  (void)rvalue;

  if (blang->goto_blocks.contains(name)) {
    blang->builder.CreateBr(blang->goto_blocks[name]);
  } else {
    /*
auto goto_block = BasicBlock::Create(blang->context, name + "_goto",
                             blang->current_function);
auto unresolved_block =
BasicBlock::Create(blang->context, name, blang->current_function);
blang->builder.CreateBr(goto_block);
blang->builder.SetInsertPoint(goto_block);
blang->builder.CreateBr(unresolved_block);
blang->goto_blocks[name] = unresolved_block;
    */
    auto unresolved_block =
        BasicBlock::Create(blang->context, name, blang->current_function);
    blang->builder.CreateBr(unresolved_block);
    blang->goto_blocks[name] = unresolved_block;
    blang->unresolved_goto_labels.emplace_back(name, label_symbol_location);
  }
  return nullptr;
}

} // namespace blang
