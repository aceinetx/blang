#include "../blang.hh"
#include "ast.hh"
#include <llvm/IR/BasicBlock.h>
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstFuncDef::print(int indent) const {
  printIndent(indent);
  std::cout << "AstFuncDef" << std::endl;
  printIndent(indent + 1);
  std::cout << "name: " << name << std::endl;
  for (const auto &stmt : children) {
    stmt->print(indent + 1);
  }
}

bool AstFuncDef::compile(Blang *blang) {
  std::vector<Type *> arg_types = {};
  for (size_t i = 0; i < args.size(); i++) {
    arg_types.push_back(blang->getBWordTy());
  }
  auto type = FunctionType::get(blang->getBWordTy(), arg_types, false);
  auto func =
      Function::Create(type, Function::ExternalLinkage, name, blang->fmodule);
  auto block = BasicBlock::Create(blang->context, "_" + name, func);

  blang->builder.SetInsertPoint(block);

  blang->scopes.push_back({});

  auto fnArgs = func->arg_begin();
  Value *arg = fnArgs++;
  for (size_t i = 0; i < args.size(); i++) {
    {
      auto var = std::make_unique<Variable>();
      var->name = args[i];
      var->value =
          blang->builder.CreateAlloca(blang->getBWordTy(), nullptr, args[i]);
      blang->builder.CreateStore(arg, var->value);

      blang->scopes.back().variables.push_back(std::move(var));
    }

    arg = fnArgs++;
  }

  blang->current_func = func;
  if (!AstNode::compile(blang))
    return false;

  if (!block->getTerminator()) {
    blang->builder.CreateRet(ConstantInt::get(blang->getBWordTy(), 0));
  }

  blang->scopes.pop_back();
  while (!blang->values.empty())
    blang->values.pop();

  return true;
}
