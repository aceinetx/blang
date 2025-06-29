#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;
using namespace llvm;

void AstFuncCall::print(int indent) const {
  printIndent(indent);
  std::cout << "AstFuncCall" << std::endl;
  printIndent(indent + 1);
  std::cout << "name: " << name << std::endl;
  for (AstNode *arg : args) {
    arg->print(indent + 1);
  }
}

AstFuncCall::~AstFuncCall() {
  for (AstNode *arg : args) {
    delete arg;
  }
}

bool AstFuncCall::compile(Blang *blang) {
  auto func = blang->fmodule.getFunction(name);
  if (!func) {
    if (blang->extern_symbols.count(name)) {
      ExternSymbol &ext = blang->extern_symbols[name];
      ext.isFunction = true;

      if (!ext.isExterned) {
        Type *returnType = blang->getBWordTy();
        FunctionType *functionType = FunctionType::get(returnType, {}, true);
        Function *externalFunction = Function::Create(
            functionType, Function::ExternalLinkage, name, blang->fmodule);
        externalFunction->addFnAttr(Attribute::NoInline);
        func = externalFunction;

        ext.isExterned = true;
      }
    } else {
      // blang->compile_error = fmt::format("undefined function: {}", name);
      // return false;
    }
  }

  std::vector<Value *> arg_values = {};
  for (auto arg : args) {
    blang->expr_types.push(Blang::RVALUE);
    if (!arg->compile(blang))
      return false;
    arg_values.push_back(blang->values.top());
    blang->values.pop();
  }

  if (!func) {
    std::vector<Type *> art;
    for (AstNode *arg : args)
      art.push_back(blang->getBWordTy());
    Type *returnType = blang->getBWordTy();
    FunctionType *functionType = FunctionType::get(returnType, art, false);
    auto f = blang->fmodule.getOrInsertFunction(name, functionType);
    blang->values.push(blang->builder.CreateCall(f, arg_values));
  } else {
    blang->values.push(blang->builder.CreateCall(func, arg_values));
  }

  if (!blang->expr_types.empty()) {
    blang->expr_types.pop();
  }

  return true;
}
