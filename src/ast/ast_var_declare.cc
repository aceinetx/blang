#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;

void AstVarDeclare::print(int indent) const {
  printIndent(indent);
  std::cout << "AstVarDeclare" << std::endl;
  printIndent(indent + 1);
  std::cout << "names: ";
  for (std::string name : names) {
    std::cout << name << " ";
  }
  std::cout << std::endl;
}

bool AstVarDeclare::compile(Blang *blang) {
  for (std::string name : names) {
    auto var = std::make_unique<Variable>();
    var->name = name;
    var->value =
        blang->builder.CreateAlloca(blang->builder.getInt64Ty(), nullptr, name);

    blang->scopes.back().variables.push_back(std::move(var));
  }

  return true;
}
