#include "frontend/ast/AstFuncDef.hh"
#include "Blang.hh"
#include "frontend/exceptions/UnresolvedLabelException/UnresolvedLabelException.hh"
#include <fmt/core.h>
#include <llvm/IR/DerivedTypes.h>

using namespace llvm;

namespace blang {
void AstFuncDef::print(int indent) {
  printIndent(indent);
  fmt::print("- AstFuncDef\n");
  node_block->print(indent + 1);
}

llvm::Value *AstFuncDef::compile(Blang *blang, bool rvalue) {
  (void)rvalue;

  std::vector<Type *> arg_types = {};
  for (size_t i = 0; i < args.size(); i++)
    arg_types.push_back(blang->get_word_ty());

  auto type = FunctionType::get(blang->get_word_ty(), arg_types, false);
  auto func =
      Function::Create(type, Function::ExternalLinkage, name, blang->fmodule);
  auto function_block = BasicBlock::Create(blang->context, "_" + name, func);
  blang->builder.SetInsertPoint(function_block);

  blang->add_global_scope_var(name, func);

  blang->push_scope();
  blang->current_function = func;
  blang->unresolved_goto_labels.clear();
  blang->goto_blocks.clear();
  blang->while_statement_end_block = nullptr;

  /* Initialize arguments */
  auto fnArgs = func->arg_begin();
  Value *arg = fnArgs++;
  for (const auto &i : args) {
    {
      auto var = blang->builder.CreateAlloca(blang->get_word_ty(), nullptr, i);
      blang->builder.CreateStore(arg, var);
      blang->add_scope_var(i, var, location);
    }

    arg = fnArgs++;
  }

  /* Compile body */
  llvm::Value *last = node_block->compile(blang, true);

  blang->pop_scope();
  blang->current_function = nullptr;

  /*
   * Check for unresolved goto labels
   */
  for (const auto &label : blang->unresolved_goto_labels) {
    throw UnresolvedLabelException(label.second, label.first);
  }

  /*
   * When we encounter an unconditional branch we want to remove any
   * instructions that come after it until a label is encountered. This is
   * needed because goto can do unconditional jumps whenever the user wants to.
   * LLVM IR doesn't really like that, it expects any block to be terminated by
   * a control flow instruction and by one control flow instruction only; it
   * also expects anonymous instructions to be numbered in the order of the
   * control flow, so that also breaks */
  for (BasicBlock &BB : *func) {
    for (auto it = BB.begin(); it != BB.end(); ++it) {
      if (auto *BI = dyn_cast<BranchInst>(&*it)) {
        if (BI->isUnconditional()) {
          ++it; // move to instruction after the branch
          while (it != BB.end()) {
            Instruction &I = *it++;
            I.eraseFromParent();
          }
          break; // done with this block
        }
      }
    }
  }

  if (!blang->builder.GetInsertBlock()->getTerminator())
    blang->builder.CreateRet(ConstantInt::get(blang->get_word_ty(), 0));

  return last;
}
} // namespace blang
