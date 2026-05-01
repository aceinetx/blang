#include "frontend/ast/AstGlobalArray.hh"
#include "Assert.hh"
#include "CompilerContext.hh"
#include <algorithm>
#include <fmt/core.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/Support/Casting.h>

using namespace llvm;

namespace blang {
void AstGlobalArray::print(int indent) {
  printIndent(indent);
  fmt::print("- AstGlobalArray {}[{}]\n", name, size);
  for (auto value : values)
    value->print(indent + 1);
}

llvm::Value *AstGlobalArray::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;

  blangassert(size > 0 || values.size() > 0);

  size_t needed_size = std::max((size_t)size, values.size());

  auto type = C->get_word_ty();
  auto arrayType = ArrayType::get(type, needed_size);

  std::vector<Constant *> consts;
  for (const auto &value : values) {
    consts.push_back(dyn_cast<Constant>(value->compile(C, true)));
  }

  auto array = ConstantArray::get(arrayType, consts);
  auto var = new GlobalVariable(
      C->fmodule, C->get_word_ty(), false, llvm::GlobalValue::ExternalLinkage,
      new GlobalVariable(C->fmodule, C->get_word_ty(), false,
                         llvm::GlobalValue::ExternalLinkage, array),
      name);

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

void AstGlobalArray::bindings(std::ostream &os) {
  os << "extern int64_t *" << name << ";\n";
}

} // namespace blang
