#include "frontend/ast/AstGlobalVar.hh"
#include "CompilerContext.hh"
#include <fmt/core.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/Support/Casting.h>

using namespace llvm;

namespace blang {
void AstGlobalVar::print(int indent) {
  printIndent(indent);
  fmt::println("- AstGlobalVar {}", name);
  for (auto value : values)
    value->print(indent + 1);
}

llvm::Value *AstGlobalVar::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;

  if (values.empty()) {
    auto var = new GlobalVariable(C->fmodule, C->get_word_ty(), false,
                                  llvm::GlobalValue::ExternalLinkage,
                                  ConstantInt::get(C->get_word_ty(), 0), name);

    C->add_global_scope_var(name, var, location);
  } else if (values.size() == 1) {
    auto value = values[0]->compile(C, true);
    auto var = new GlobalVariable(C->fmodule, C->get_word_ty(), false,
                                  llvm::GlobalValue::ExternalLinkage,
                                  dyn_cast<Constant>(value), name);

    C->add_global_scope_var(name, var, location);
  } else {
    auto type = C->get_word_ty();
    auto arrayType = ArrayType::get(type, values.size());

    std::vector<Constant *> consts;
    for (const auto &value : values) {
      consts.push_back(dyn_cast<Constant>(value->compile(C, true)));
    }

    auto array = ConstantArray::get(arrayType, consts);
    auto var =
        new GlobalVariable(C->fmodule, C->get_word_ty(), false,
                           llvm::GlobalValue::ExternalLinkage, array, name);

    C->add_global_scope_var(name, var, location);
  }
  return nullptr;
}

} // namespace blang
