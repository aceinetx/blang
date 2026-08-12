#include "frontend/ast/AstFuncDef.hh"
#include "CompilerContext.hh"
#include "frontend/exceptions/InvalidAttributeException/InvalidAttributeException.hh"
#include "frontend/exceptions/UnresolvedLabelException/UnresolvedLabelException.hh"
#include <fmt/core.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <unordered_map>

using namespace llvm;

namespace blang {
AstFuncDef::AstFuncDef() : args(std::make_shared<AstIdentifierList>()) {
}

void AstFuncDef::print(int indent) {
  printIndent(indent);
  fmt::print("- AstFuncDef\n");
  args->print(indent + 1);
  node_block->print(indent + 1);
}

llvm::Value *AstFuncDef::compile(CompilerContext *C, bool rvalue) {
  (void)rvalue;

  std::vector<Type *> arg_types = {};
  for (size_t i = 0; i < args->identifiers.size(); i++)
    arg_types.push_back(C->get_word_ty());

  auto type = FunctionType::get(C->get_word_ty(), arg_types, false);
  auto func =
      Function::Create(type, Function::ExternalLinkage, name, C->fmodule);

  auto attr_list = AttributeList();
  std::unordered_map<std::string, Attribute::AttrKind> attribute_map = {
      {"inline", Attribute::AlwaysInline},
      {"noreturn", Attribute::NoReturn},
      {"naked", Attribute::Naked},
      {"norecurse", Attribute::NoRecurse},
  };

  for (const auto &attr : attrs) {
    if (attribute_map.contains(attr)) {
      attr_list = attr_list.addFnAttribute(C->context, attribute_map[attr]);
    } else
      throw InvalidAttributeException(location, attr);
  }
  func->setAttributes(attr_list);

  auto function_block = BasicBlock::Create(C->context, "_" + name, func);
  C->builder.SetInsertPoint(function_block);

  // setup debug info
  if (C->debug_enabled && C->di_builder && C->di_file) {
    auto line_no = static_cast<unsigned>(location.begin.line);

    llvm::SmallVector<llvm::Metadata *, 64> param_types;
    auto *word_di_type = C->get_word_di_type();
    param_types.push_back(word_di_type);
    for (size_t i = 0; i < args->identifiers.size(); i++)
      param_types.push_back(word_di_type);

    auto *subroutine_type = C->di_builder->createSubroutineType(
        C->di_builder->getOrCreateTypeArray(param_types));

    auto *sp = C->di_builder->createFunction(
        C->di_file,      // scope
        name,            // name
        name,            // linkage name
        C->di_file,      // file
        line_no,         // line number
        subroutine_type, // type
        line_no,         // scope line
        llvm::DINode::FlagPrototyped, llvm::DISubprogram::SPFlagDefinition);

    func->setSubprogram(sp);
    C->di_current_subprogram = sp;
    C->set_debug_location(location);
  }

  // if user forward declared the function that is in this module then we
  // replace all references with the real function and remove the extern so that
  // we don't get duplicate symbols
  if (C->extern_values.contains(name)) {
    auto *stub = C->extern_values[name];
    llvm::cast<GlobalVariable>(stub)->eraseFromParent();
    stub->replaceAllUsesWith(func);
    C->extern_values.erase(name);
    C->update_global_scope_var(name, func);

    // without this the function name would clash with the extern symbol, this
    // happens because we create the function before we delete the extern symbol
    // above
    func->setName(name);
  } else {
    C->add_global_scope_var(name, func);
  }

  C->push_scope();
  C->current_function = func;
  C->unresolved_goto_labels.clear();
  C->goto_blocks.clear();
  C->while_statement_end_block = nullptr;

  /* Initialize arguments */
  auto fnArgs = func->arg_begin();
  Value *arg = fnArgs++;
  for (size_t i = 0; i < args->identifiers.size(); i++) {
    const auto &name = args->identifiers[i].first;
    const auto &location = args->identifiers[i].second;

    {
      auto var = C->builder.CreateAlloca(C->get_word_ty(), nullptr, name);
      C->builder.CreateStore(arg, var);
      C->add_scope_var(name, var, location);

      C->emit_local_var_debug_info(var, name, location, true,
                                   static_cast<unsigned>(i + 1));
    }

    arg = fnArgs++;
  }

  /* Compile body */
  llvm::Value *last = node_block->compile(C, true);

  C->pop_scope();
  C->current_function = nullptr;

  if (C->debug_enabled) {
    C->di_current_subprogram = nullptr;
    C->clear_debug_location();
  }

  /*
   * Check for unresolved goto labels
   */
  for (const auto &label : C->unresolved_goto_labels) {
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

  if (!C->builder.GetInsertBlock()->getTerminator())
    C->builder.CreateRet(ConstantInt::get(C->get_word_ty(), 0));

  return last;
}

void AstFuncDef::bindings(std::ostream &os) {
  os << "extern int64_t " << name << " (int64_t arg, ...);\n";

  os << "#define " << name << "(";
  for (const auto &arg : args->identifiers) {
    if (&arg != &args->identifiers[0]) {
      os << ",";
    }
    os << arg.first;
  }
  os << ") " << name << "(";
  for (const auto &arg : args->identifiers) {
    if (&arg != &args->identifiers[0]) {
      os << ",";
    }
    os << "(int64_t)(" << arg.first << ")";
  }
  os << ")\n";
}

} // namespace blang
