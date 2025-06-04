#pragma once
#include "result.hh"
#include "yy.hh"
#include <pch.hh>

namespace blang {

typedef struct Variable {
  std::string name;
  llvm::Value *value;
} Variable;

typedef struct ExternSymbol {
  std::string name;
  bool isFunction;
  bool isExterned;
} ExternSymbol;

typedef struct Scope {
  std::vector<std::unique_ptr<Variable>> variables;

  Scope();
} Scope;

typedef struct If {
  llvm::BasicBlock *true_block;
  llvm::BasicBlock *false_block;
  llvm::BasicBlock *merge_block;
  bool elif = false;
} If;

typedef struct While {
  llvm::BasicBlock *cmp;
  llvm::BasicBlock *body;
  llvm::BasicBlock *end;
} While;

class Blang {
public:
  Parser *parser;
  std::string input;
  std::string compile_error;

  std::string targetTriple;
  llvm::Target *target;
  llvm::TargetMachine *targetMachine;

  llvm::LLVMContext context;
  llvm::IRBuilder<> builder;
  llvm::Module fmodule;

  Scope global_scope;
  std::vector<Scope> scopes;
  std::stack<llvm::Value *> values;
  std::map<std::string, ExternSymbol> extern_symbols;

  std::stack<If> ifs;
  std::stack<llvm::BasicBlock *> if_blocks;
  unsigned long ifID;

  std::stack<While> whiles;
  unsigned long whileID;

  llvm::Function *current_func;

  std::vector<std::string> link_libraries;
  std::string link_path;

  enum ExprType {
    LVALUE,
    RVALUE,
  };
  std::stack<ExprType> expr_types;

  Blang(std::string moduleName);
  ~Blang();

  Result<NoSuccess, std::string> parseAndCompile();
  Variable *getVariable(std::string name);

  llvm::Type *getBWordTy();

  enum EmitLevel {
    EMIT_EXE,
    EMIT_OBJ,
    EMIT_IR,
  };

  Result<NoSuccess, std::string> emit(std::string filename, EmitLevel level);
};
} // namespace blang
