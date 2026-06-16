#include "frontend/ast/AstLabel.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstLabel::print(int indent) {
  printIndent(indent);
  fmt::print("- AstLabel {}\n", name);
}

fir::Value AstLabel::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;

  if (!C->goto_blocks.contains(name)) {
    auto block = C->ir.block();
    C->ir.br(block);
    C->ir.set_insert_point(block);
    C->goto_blocks[name] = block;
  } else {
    C->ir.br(C->goto_blocks[name]);
    C->ir.set_insert_point(C->goto_blocks[name]);

    for (ssize_t i = 0; i < (ssize_t)C->unresolved_goto_labels.size(); i++) {
      if (C->unresolved_goto_labels[i].first == name) {
        C->unresolved_goto_labels.erase(C->unresolved_goto_labels.begin() + i);
      }
    }
  }
  return {0};
}

} // namespace blang
