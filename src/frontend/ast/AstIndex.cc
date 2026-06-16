#include "frontend/ast/AstIndex.hh"
#include "Assert.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>

namespace blang {
void AstIndex::print(int indent) {
  printIndent(indent);
  fmt::print("- AstIndex\n");
  expression->print(indent + 1);
  index->print(indent + 1);
}

fir::Value AstIndex::compile(CompilerContext *C, bool rvalue) {
#if 0
  auto *array = C->builder.CreateIntToPtr(expression->compile(C, true),
                                          C->builder.getPtrTy());

  auto *index_value = index->compile(C, true);
  auto *element = C->builder.CreateGEP(C->get_word_ty(), array, index_value);
  if (rvalue)
    element = C->builder.CreateLoad(C->get_word_ty(), element);
  else
    element = C->builder.CreatePtrToInt(element, C->get_word_ty());

  return element;
#endif
  blangassert(0 && "Unsupported");
  return {0};
}

} // namespace blang
