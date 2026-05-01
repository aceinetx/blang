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

  GlobalVariable *var = nullptr;
  if (values.empty()) {
    var = new GlobalVariable(C->fmodule, C->get_word_ty(), false,
                             llvm::GlobalValue::ExternalLinkage,
                             ConstantInt::get(C->get_word_ty(), 0), name);

  } else if (values.size() == 1) {
    auto value = values[0]->compile(C, true);
    var = new GlobalVariable(C->fmodule, C->get_word_ty(), false,
                             llvm::GlobalValue::ExternalLinkage,
                             dyn_cast<Constant>(value), name);
  } else {
    auto type = C->get_word_ty();
    auto arrayType = ArrayType::get(type, values.size());

    std::vector<Constant *> consts;
    for (const auto &value : values) {
      consts.push_back(dyn_cast<Constant>(value->compile(C, true)));
    }

    auto array = ConstantArray::get(arrayType, consts);
    var = new GlobalVariable(C->fmodule, C->get_word_ty(), false,
                             llvm::GlobalValue::ExternalLinkage, array, name);
  }

  // if user forward declared the variable that is in this module then we
  // replace all references with the real variable and remove the extern so that
  // we don't get duplicate symbols
  if (C->extern_values.contains(name)) {
    auto *stub = C->extern_values[name];
    llvm::cast<GlobalVariable>(stub)->eraseFromParent();
    stub->replaceAllUsesWith(var);
    C->extern_values.erase(name);
    C->update_global_scope_var(name, var);

    // without this the variable name would clash with the extern symbol, this
    // happens because we create the variable before we delete the extern symbol
    // above
    var->setName(name);
  } else {
    C->add_global_scope_var(name, var, location);
  }
  return nullptr;
}

} // namespace blang
