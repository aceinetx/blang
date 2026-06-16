#include "frontend/ast/AstGoto.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstGoto::print(int indent) {
  printIndent(indent);
  fmt::print("- AstGoto {}\n", name);
}

fir::Value AstGoto::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;
  if (C->goto_blocks.contains(name)) {
    C->ir.br(C->goto_blocks[name]);
  } else {
    /*
auto goto_block = BasicBlock::Create(C->context, name + "_goto",
                             C->current_function);
auto unresolved_block =
BasicBlock::Create(C->context, name, C->current_function);
C->builder.CreateBr(goto_block);
C->builder.SetInsertPoint(goto_block);
C->builder.CreateBr(unresolved_block);
C->goto_blocks[name] = unresolved_block;
    */
    auto unresolved_block = C->ir.block();
    C->ir.br(unresolved_block);
    C->goto_blocks[name] = unresolved_block;
    C->unresolved_goto_labels.emplace_back(name, label_symbol_location);
  }

  return {0};
}

} // namespace blang
