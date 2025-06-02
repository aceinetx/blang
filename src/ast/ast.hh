#pragma once
#include <pch.hh>

namespace blang {
class Blang;
class AstNode {
public:
  virtual ~AstNode() = 0;
  virtual void print(int indent = 0) const = 0;
  virtual bool compile(Blang *blang);

  std::vector<AstNode *> children;

protected:
  void printIndent(int indent) const;
};

class AstRootNode : public AstNode {
public:
  void print(int indent = 0) const override;
};

class AstFuncDef : public AstNode {
public:
  std::string name;
  std::vector<std::string> args;

  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;
};

class AstVarDeclare : public AstNode {
public:
  std::vector<std::string> names;
  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;
};

class AstVarAssign : public AstNode {
public:
  AstNode *lexpr;
  AstNode *rexpr;
  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;
  ~AstVarAssign();
};

class AstNumber : public AstNode {
public:
  int value;
  explicit AstNumber(int val) : value(val) {
  }
  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;
};

class AstBinaryOp : public AstNode {
public:
  AstNode *left;
  AstNode *right;
  std::string op;

  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;

  ~AstBinaryOp();
};

class AstReturn : public AstNode {
public:
  AstNode *expr;
  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;

  ~AstReturn();
};

class AstVarRef : public AstNode {
public:
  std::string name;
  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;
};

class AstFuncCall : public AstNode {
public:
  std::vector<AstNode *> args;
  std::string name;

  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;

  ~AstFuncCall();
};

class AstExtrn : public AstNode {
public:
  std::vector<std::string> names;

  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;
};

class AstStrRef : public AstNode {
public:
  std::string str;

  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;
};

class AstArrIndex : public AstNode {
public:
  AstNode *expr;
  AstNode *index;

  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;

  ~AstArrIndex();
};

class AstDeref : public AstNode {
public:
  AstNode *expr;
  int times;

  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;

  ~AstDeref();
};

class AstAddrof : public AstNode {
public:
  AstNode *expr;
  int times;

  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;

  ~AstAddrof();
};

class AstIf : public AstNode {
public:
  std::vector<AstNode *> body;
  AstNode *expr;

  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;

  ~AstIf();
};

class AstElif : public AstNode {
public:
  std::vector<AstNode *> body;
  AstNode *expr;

  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;

  ~AstElif();
};

class AstElse : public AstNode {
public:
  std::vector<AstNode *> body;

  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;

  ~AstElse();
};

class AstIfChain : public AstNode {
public:
  std::vector<AstNode *> ifs;

  void print(int indent = 0) const override;
  bool compile(Blang *blang) override;

  ~AstIfChain();
};
} // namespace blang
