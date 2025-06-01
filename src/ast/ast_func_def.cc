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

bool AstFuncDef::compile(Blang *bc) {
  std::vector<Type *> arg_types = {};
  for (size_t i = 0; i < args.size(); i++) {
    arg_types.push_back(bc->getBWordTy());
  }
  auto type = FunctionType::get(bc->getBWordTy(), arg_types, false);
  auto func =
      Function::Create(type, Function::ExternalLinkage, name, bc->fmodule);
  auto block = BasicBlock::Create(bc->context, "_" + name, func);

  bc->builder.SetInsertPoint(block);

  bc->scopes.push_back({});

  auto fnArgs = func->arg_begin();
  Value *arg = fnArgs++;
  for (size_t i = 0; i < args.size(); i++) {
    {
      auto var = std::make_unique<Variable>();
      var->name = args[i];
      var->value = bc->builder.CreateAlloca(bc->getBWordTy(), nullptr, args[i]);
      bc->builder.CreateStore(arg, var->value);

      bc->scopes.back().variables.push_back(std::move(var));
    }

    arg = fnArgs++;
  }

  if (!AstNode::compile(bc))
    return false;

  if (!block->getTerminator()) {
    bc->builder.CreateRet(ConstantInt::get(bc->getBWordTy(), 0));
  }

  bc->scopes.pop_back();
  while (!bc->values.empty())
    bc->values.pop();

  return true;
}
