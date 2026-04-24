#include "frontend/ast/AstIndex.hh"
#include "Blang.hh"
#include <fmt/base.h>

using namespace llvm;

namespace blang {
void AstIndex::print(int indent) {
  printIndent(indent);
  fmt::println("- AstIndex");
  expression->print(indent + 1);
  index->print(indent + 1);
}

llvm::Value *AstIndex::compile(Blang *blang, bool rvalue) {
  auto *array = blang->builder.CreateIntToPtr(expression->compile(blang, true),
                                              blang->builder.getPtrTy());

  auto *index_value = index->compile(blang, true);
  auto *element =
      blang->builder.CreateGEP(blang->get_word_ty(), array, index_value);
  if (rvalue)
    element = blang->builder.CreateLoad(blang->get_word_ty(), element);
  else
    element = blang->builder.CreatePtrToInt(element, blang->get_word_ty());

  return element;
}

} // namespace blang
