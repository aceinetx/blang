#include "frontend/ast/AstFuncDef.hh"
#include "Blang.hh"
#include <fmt/base.h>
#include <llvm/IR/DerivedTypes.h>

using namespace llvm;

namespace blang {
void AstFuncDef::print(int indent) {
  printIndent(indent);
  fmt::println("- AstFuncDef");
  for (auto child : statements) {
    child->print(indent + 1);
  }
}

llvm::Value *AstFuncDef::compile(Blang *blang) {
  auto type =
      FunctionType::get(blang->get_word_ty(), std::vector<Type *>(), false);
  auto func =
      Function::Create(type, Function::ExternalLinkage, name, blang->fmodule);
  auto block = BasicBlock::Create(blang->context, "_" + name, func);
  blang->builder.SetInsertPoint(block);

  blang->push_scope();

  llvm::Value *last = nullptr;
  for (auto child : statements) {
    last = child->compile(blang);
  }

  blang->pop_scope();

  return last;
}
} // namespace blang
