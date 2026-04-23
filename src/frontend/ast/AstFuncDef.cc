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
  std::vector<Type *> arg_types = {};
  for (size_t i = 0; i < args.size(); i++)
    arg_types.push_back(blang->get_word_ty());

  auto type = FunctionType::get(blang->get_word_ty(), arg_types, false);
  auto func =
      Function::Create(type, Function::ExternalLinkage, name, blang->fmodule);
  auto block = BasicBlock::Create(blang->context, "_" + name, func);
  blang->builder.SetInsertPoint(block);

  blang->add_global_scope_var(name, func);

  blang->push_scope();
  blang->current_function = func;
  blang->goto_blocks.clear();

  /* Initialize arguments */
  auto fnArgs = func->arg_begin();
  Value *arg = fnArgs++;
  for (const auto &i : args) {
    {
      auto var = blang->builder.CreateAlloca(blang->get_word_ty(), nullptr, i);
      blang->builder.CreateStore(arg, var);
      blang->add_scope_var(i, var);
    }

    arg = fnArgs++;
  }

  /* Compile body */
  llvm::Value *last = nullptr;
  for (auto child : statements) {
    last = child->compile(blang);
  }

  blang->pop_scope();
  blang->current_function = nullptr;

  return last;
}
} // namespace blang
