#pragma once
#include "parser/parser.hh"
#include "result.hh"
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
  llvm::BasicBlock *fallthrough;
  std::vector<llvm::BasicBlock *> parts;
  llvm::BasicBlock *end;
} If;

class Blang {
public:
  Lexer lexer;
  Parser parser;

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
  unsigned long ifID;

  llvm::Function *current_func;

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
    EMIT_OBJ,
    EMIT_IR,
  };

  Result<NoSuccess, std::string> emit(std::string filename, EmitLevel level);
};
} // namespace blang
