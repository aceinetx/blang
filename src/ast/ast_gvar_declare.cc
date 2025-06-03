#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;

void AstGvarDeclare::print(int indent) const {
  printIndent(indent);
  std::cout << "AstGvarDeclare" << std::endl;
  printIndent(indent + 1);
  std::cout << "name: " << name << "\n";
}

bool AstGvarDeclare::compile(Blang *blang) {
  llvm::GlobalVariable *var = new llvm::GlobalVariable(
      blang->fmodule, blang->getBWordTy(), false,
      llvm::GlobalValue::ExternalLinkage, nullptr, name);

  auto variable = std::make_unique<Variable>();
  variable->name = name;
  variable->value = var;

  blang->scopes.begin()->variables.push_back(std::move(variable));

  return true;
}
