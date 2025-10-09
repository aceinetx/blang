#include "../blang.hh"
#include "ast.hh"
#include <pch.hh>

using namespace blang;

template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void AstGvarDeclare::print(int indent) const {
  printIndent(indent);
  std::cout << "AstGvarDeclare" << std::endl;
  printIndent(indent + 1);
  std::cout << "name: " << name << "\n";
}

template <typename T>
llvm::Constant *visit_value(AstGvarDeclare *ast, Blang *blang, T value) {
  llvm::Constant *v;
  std::visit(
      overloaded{
          [&v, blang](auto n) {
            v = llvm::ConstantInt::get(blang->getBWordTy(), 0);
          },
          [&v, blang](long number) {
            v = llvm::ConstantInt::get(blang->getBWordTy(), number);
          },
          [&v, blang](std::string s) {
            v = blang->builder.CreateGlobalStringPtr(
                AstStrRef::ParseEscapeSequences(s), "", 0U, &blang->fmodule);
          },
          [&v, blang, ast](std::vector<AstGvarDeclare::value_single_t> vec) {
            auto arrayType =
                llvm::ArrayType::get(blang->getBWordTy(), vec.size());
            std::vector<llvm::Constant *> values;
            for (auto value : vec) {
              values.push_back(visit_value(ast, blang, value));
            }

            v = llvm::ConstantArray::get(arrayType, values);
            ast->isArray = true;
          }},
      value);
  return v;
}

template <typename T> llvm::Type *visit_value_type(Blang *blang, T value) {
  llvm::Type *t;
  std::visit(
      overloaded{[&t, blang](auto n) { t = blang->getBWordTy(); },
                 [&t, blang](long number) { t = blang->getBWordTy(); },
                 [&t, blang](std::string s) { t = blang->getBWordTy(); },
                 [&t, blang](std::vector<AstGvarDeclare::value_single_t> vec) {
                   auto arrayType =
                       llvm::ArrayType::get(blang->getBWordTy(), vec.size());
                   t = arrayType;
                 }},
      value);
  return t;
}

bool AstGvarDeclare::compile(Blang *blang) {
  llvm::Constant *defaultValue = visit_value(this, blang, value);
  llvm::Type *type = visit_value_type(blang, value);
  llvm::GlobalVariable *var;
  if (isArray) {
    auto array = new llvm::GlobalVariable(blang->fmodule, type, false,
                                          llvm::GlobalValue::ExternalLinkage,
                                          defaultValue);
    var = new llvm::GlobalVariable(blang->fmodule, type, false,
                                   llvm::GlobalValue::ExternalLinkage, array,
                                   name);
  } else {
    var = new llvm::GlobalVariable(blang->fmodule, type, false,
                                   llvm::GlobalValue::ExternalLinkage,
                                   defaultValue, name);
  }

  auto variable = std::make_unique<Variable>();
  variable->name = name;
  variable->value = var;

  blang->scopes.begin()->variables.push_back(std::move(variable));

  return true;
}
