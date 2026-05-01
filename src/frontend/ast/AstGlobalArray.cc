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
  fmt::println("- AstGlobalArray {}[{}]", name, size);
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
  auto var =
      new GlobalVariable(C->fmodule, C->get_word_ty(), false,
                         llvm::GlobalValue::ExternalLinkage, array, name);
  auto var_ptr =
      new GlobalVariable(C->fmodule, C->get_word_ty(), false,
                         llvm::GlobalValue::ExternalLinkage, var, name);

  C->add_global_scope_var(name, var_ptr, location);

  return nullptr;
}

} // namespace blang
