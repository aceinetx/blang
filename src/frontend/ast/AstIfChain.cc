#include "frontend/ast/AstIfChain.hh"
#include "Blang.hh"
#include <fmt/core.h>

using namespace llvm;

namespace blang {
void AstIfChain::print(int indent) {
  printIndent(indent);
  fmt::print("- AstIfChain\n");
  begin_if->print(indent + 1);
  for (auto node : elifs) {
    node->print(indent + 1);
  }
  if (end_else)
    end_else->print(indent + 1);
}

llvm::Value *AstIfChain::compile(Blang *blang, bool rvalue) {
  (void)rvalue;

  auto *value =
      blang->builder.CreateICmpNE(begin_if->expression->compile(blang, true),
                                  ConstantInt::get(blang->get_word_ty(), 0));

  /* Create body blocks */
  auto *begin_if_block =
      BasicBlock::Create(blang->context, "", blang->current_function);

  std::vector<BasicBlock *> elif_blocks;
  for (auto elif : elifs) {
    elif_blocks.push_back(
        BasicBlock::Create(blang->context, "", blang->current_function));
  }

  BasicBlock *end_else_block = nullptr;
  if (end_else)
    end_else_block =
        BasicBlock::Create(blang->context, "", blang->current_function);

  /* Create a block for the end of the chain */
  auto *chain_end_block =
      BasicBlock::Create(blang->context, "", blang->current_function);

  if (begin_if_block && elifs.empty() && !end_else) {
    /* Only one if statement */
    blang->builder.CreateCondBr(value, begin_if_block, chain_end_block);
  } else if (begin_if_block && elifs.empty() && end_else) {
    /* Only an if and else statement */
    blang->builder.CreateCondBr(value, begin_if_block, end_else_block);
  } else if (begin_if_block && !elifs.empty() && !end_else) {
    /* If and elif statements */
    auto *skip =
        BasicBlock::Create(blang->context, "", blang->current_function);
    blang->builder.CreateCondBr(value, begin_if_block, skip);

    blang->builder.SetInsertPoint(skip);
    for (size_t i = 0; i < elifs.size(); i++) {
      auto block = elif_blocks[i];
      auto elif = elifs[i];
      skip = BasicBlock::Create(blang->context, "", blang->current_function);
      value = blang->builder.CreateICmpNE(
          elif->expression->compile(blang, true),
          ConstantInt::get(blang->get_word_ty(), 0));
      blang->builder.CreateCondBr(value, block, skip);
      blang->builder.SetInsertPoint(skip);
    }

    blang->builder.SetInsertPoint(skip);
    blang->builder.CreateBr(chain_end_block);
  } else if (begin_if_block && !elifs.empty() && end_else) {
    /* If and else and elif statements */
    auto *skip =
        BasicBlock::Create(blang->context, "", blang->current_function);
    blang->builder.CreateCondBr(value, begin_if_block, skip);

    blang->builder.SetInsertPoint(skip);
    for (size_t i = 0; i < elifs.size(); i++) {
      auto block = elif_blocks[i];
      auto elif = elifs[i];
      skip = BasicBlock::Create(blang->context, "", blang->current_function);
      value = blang->builder.CreateICmpNE(
          elif->expression->compile(blang, true),
          ConstantInt::get(blang->get_word_ty(), 0));
      blang->builder.CreateCondBr(value, block, skip);
      blang->builder.SetInsertPoint(skip);
    }

    blang->builder.SetInsertPoint(skip);
    blang->builder.CreateBr(end_else_block);
  }

  /* Compile if body */
  blang->builder.SetInsertPoint(begin_if_block);
  begin_if->body->compile(blang, true);
  if (!blang->builder.GetInsertBlock()->getTerminator())
    blang->builder.CreateBr(chain_end_block);
  blang->builder.SetInsertPoint(chain_end_block);
  /* Compile else if bodies */
  for (size_t i = 0; i < elifs.size(); i++) {
    auto block = elif_blocks[i];
    auto node = elifs[i];

    blang->builder.SetInsertPoint(block);
    node->body->compile(blang, true);
    if (!blang->builder.GetInsertBlock()->getTerminator())
      blang->builder.CreateBr(chain_end_block);
    blang->builder.SetInsertPoint(chain_end_block);
  }
  if (end_else) {
    /* Compile else body */
    blang->builder.SetInsertPoint(end_else_block);
    end_else->body->compile(blang, true);
    if (!blang->builder.GetInsertBlock()->getTerminator())
      blang->builder.CreateBr(chain_end_block);
    blang->builder.SetInsertPoint(chain_end_block);
  }

  return nullptr;
}

} // namespace blang
