%language "c++"
%require "3.8"
%define api.namespace {blang}
%define api.parser.class {Parser}
%define api.token.constructor
%define api.value.type variant
%define parse.error verbose
%locations

%code requires {
#include <memory>
#include <fmt/format.h>
#include "Assert.hh"

#include "frontend/ast/AstRoot.hh"
#include "frontend/ast/AstFuncDef.hh"
#include "frontend/ast/AstReturn.hh"
#include "frontend/ast/AstNumber.hh"
#include "frontend/ast/AstAutoVar.hh"
#include "frontend/ast/AstVarRef.hh"
#include "frontend/ast/AstAssign.hh"
#include "frontend/ast/AstDeref.hh"
#include "frontend/ast/AstAddrof.hh"
#include "frontend/ast/AstIndex.hh"
#include "frontend/ast/AstBinop.hh"
#include "frontend/ast/AstFuncCall.hh"
#include "frontend/ast/AstExtern.hh"
#include "frontend/ast/AstStringLit.hh"
#include "frontend/ast/AstWhile.hh"
#include "frontend/ast/AstBreak.hh"
#include "frontend/ast/AstGoto.hh"
#include "frontend/ast/AstLabel.hh"
#include "frontend/ast/AstUnot.hh"
#include "frontend/ast/AstIf.hh"
#include "frontend/ast/AstBlock.hh"
#include "frontend/ast/AstIncDec.hh"
#include "frontend/ast/AstUinv.hh"
#include "frontend/ast/AstIdentifierList.hh"
#include "frontend/ast/AstGlobalVar.hh"
#include "frontend/ast/AstSwitch.hh"
#include "frontend/ast/AstCase.hh"

namespace blang { class Driver; }
}

%parse-param { blang::Driver& driver }
%lex-param { blang::Driver& driver }

%code {
	#include "frontend/Driver.hh"
	#include "frontend/exceptions/ParserException/ParserException.hh"
	#define mknode(type, name, loc) std::shared_ptr<type> name = std::make_shared<type>(); name->location = loc;

	namespace blang {
		Parser::symbol_type yylex(Driver& driver);
	}
}

%token END 0 "end of file"
%token <long> NUMBER
%token <std::string> IDENTIFIER
%token <std::string> STRING_LIT
%token <std::string> BLANG_EXTENSION
%token <std::vector<std::string>> BLANG_ATTRIBUTE
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token SEMICOLON COLON COMMA

/* operators sorted by precedence */
%token ASSIGN
%token BITOR
%token BITAND
%token EQUAL NEQUAL 
%token GREATER LESS GREQ LSEQ 
%token BITSHL BITSHR
%token PLUS MINUS
%token MUL DIV PERCENT
%token EXCLAMATION 

%token ASSIGNMUL ASSIGNBITAND ASSIGNMINUS
%token PLUSPLUS MINUSMINUS

%token RETURN AUTO EXTRN WHILE BREAK GOTO IF ELSE SWITCH CASE /* keywords */

%type <std::shared_ptr<blang::AstIdentifierList>> function_definition_args
%type <std::shared_ptr<blang::AstFuncDef>> function_definition
%type <std::shared_ptr<blang::AstBlock>> block
%type <std::shared_ptr<blang::AstNode>> statement_base statement_expression statement
%type <std::vector<std::shared_ptr<blang::AstNode>>> statement_list
%type <std::shared_ptr<blang::AstNode>> top_statement
%type <std::vector<std::shared_ptr<blang::AstNode>>> top_statement_list
%type <std::shared_ptr<blang::AstReturn>> return
%type <std::shared_ptr<blang::AstAutoVar>> auto
%type <std::shared_ptr<blang::AstExtern>> extrn
%type <std::shared_ptr<blang::AstWhile>> while
%type <std::shared_ptr<blang::AstBreak>> break
%type <std::shared_ptr<blang::AstGoto>> goto
%type <std::shared_ptr<blang::AstLabel>> label
%type <std::shared_ptr<blang::AstIf>> if
%type <std::shared_ptr<blang::AstSwitch>> switch
%type <std::shared_ptr<blang::AstCase>> case
%type <std::shared_ptr<blang::AstGlobalVar>> global_var
%type <std::vector<std::shared_ptr<blang::AstNode>>> expression_list
%type <std::shared_ptr<blang::AstNode>> expression expression_assign expression_bitor expression_bitand expression_equality expression_compare expression_bitshift expression_additive expression_multiplicative expression_unary expression_postfix expression_primary
%type <std::shared_ptr<blang::AstIdentifierList>> identifier_list

%%

program:
	top_statement_list {
		mknode(AstRoot, root, @1);
		root->children = std::move($1);
		driver.set_root(root);
	}
	;

top_statement:
	function_definition {
		$$ = $1;
	} | global_var {
		$$ = $1;
	}
	;

top_statement_list:
	top_statement {
		$$.clear();
		$$.push_back($1);
	}
	| top_statement_list top_statement {
		$1.push_back($2);
		$$ = $1;
	}
	;

function_definition_args:
	LPAREN identifier_list RPAREN {
		$$ = $2;
	} | LPAREN RPAREN {
		mknode(AstIdentifierList, node, @1);
		$$ = node;
	}

function_definition:
	IDENTIFIER function_definition_args statement {
		mknode(AstFuncDef, node, @1);
		node->name = $1;
		node->node_block = $3;
		node->args = $2;
		$$ = node;
	} | BLANG_ATTRIBUTE IDENTIFIER function_definition_args statement {
		mknode(AstFuncDef, node, @1);
		node->name = $2;
		node->node_block = $4;
		node->args = $3;
		node->attrs = $1;
		$$ = node;
	}
	;

global_var:
	identifier_list SEMICOLON {
		mknode(AstGlobalVar, node, @1);
		node->names = $1;
		$$ = node;
	}
	;

statement_base:
	block {
		$$ = $1;
	} | return {
		$$ = $1;
	} | auto {
		$$ = $1;
	} | extrn {
		$$ = $1;
	} | while {
		$$ = $1;
	} | break {
		$$ = $1;
	} | goto {
		$$ = $1;
	} | label {
		$$ = $1;
	} | if {
		$$ = $1;
	} | switch {
		$$ = $1;
	} | case {
		$$ = $1;
	}
	;

statement_expression:
	statement_base {
		$$ = $1;
	} | expression SEMICOLON {
		$$ = $1;
	}
	;

statement:
	statement_expression {
		$$ = $1;
	}
	;

statement_list:
	statement {
		$$.clear();
		$$.push_back($1);
	}
	| statement_list statement {
		$1.push_back($2);
		$$ = $1;
	}
	;

block:
	LBRACE statement_list RBRACE {
		mknode(AstBlock, node, @1);
		node->children = $2;
		$$ = node;
	} | LBRACE RBRACE {
		mknode(AstBlock, node, @1);
		$$ = node;
	}
	;

identifier_list:
	IDENTIFIER {
		mknode(AstIdentifierList, node, @1);
		node->identifiers.push_back({$1, @1});
		$$ = node;
	} | identifier_list COMMA IDENTIFIER {
		$1->identifiers.push_back({$3, @3});
		$$ = $1;
	}
	;

/* NOTE: there *will* be a s/r conflict there which most likely shouldn't do any harm (as far as I tested). I could resolve it with enough time and tedious parsing, but that's not something I would sacrifice my precious time for */
if:
	IF LPAREN expression RPAREN statement {
		mknode(AstIf, node, @1);
		node->expression = $3;
		node->then_node = $5;
		$$ = node;
	} | IF LPAREN expression RPAREN statement ELSE statement {
		mknode(AstIf, node, @1);
		node->expression = $3;
		node->then_node = $5;
		node->else_node = $7;
		$$ = node;
	}
	;
	
return:
	RETURN LPAREN expression RPAREN SEMICOLON {
		mknode(AstReturn, node, @1);
		node->expression = $3;
		$$ = node;
	}
	;

extrn:
	EXTRN identifier_list SEMICOLON {
		mknode(AstExtern, node, @1);
		node->names = $2;
		$$ = node;
	}
	;

auto:
	AUTO identifier_list SEMICOLON {
		mknode(AstAutoVar, node, @1);
		node->names = $2;
		$$ = node;
	}
	;

while:
	WHILE LPAREN expression RPAREN statement {
		mknode(AstWhile, node, @1);
		node->expression = $3;
		node->block = $5;
		$$ = node;
	}
	;

goto:
	GOTO IDENTIFIER SEMICOLON {
		mknode(AstGoto, node, @1);
		node->name = $2;
		node->label_symbol_location = @2;
		$$ = node;
	}
	;

label:
	IDENTIFIER COLON {
		mknode(AstLabel, node, @1);
		node->name = $1;
		$$ = node;
	}
	;

break:
	BREAK SEMICOLON {
		mknode(AstBreak, node, @1);
		$$ = node;
	}
	;

switch:
	SWITCH expression statement_base {
		mknode(AstSwitch, node, @1);
		node->expression = $2;
		node->statement = $3;
		$$ = node;
	}
	;

case:
	CASE NUMBER COLON {
		mknode(AstCase, node, @1);
		node->number = $2;
		$$ = node;
	}
	;

expression_list:
	expression {
		$$.clear();
		$$.push_back($1);
	}
	| expression_list COMMA expression {
		$1.push_back($3);
		$$ = $1;
	}
	;

expression:
	expression_assign {
		$$ = $1;
	}
	;

expression_assign:
	expression_bitor {
		$$ = $1;
	} | expression_bitor ASSIGN expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;
		node->src = $3;
		$$ = node;
	} | expression_bitor ASSIGN BITOR expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $4;
		binop->op = AstBinop::BITOR;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGNBITAND expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $3;
		binop->op = AstBinop::BITAND;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGN EQUAL expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $4;
		binop->op = AstBinop::EQUAL;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGN NEQUAL expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $4;
		binop->op = AstBinop::NEQUAL;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGN LESS expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $4;
		binop->op = AstBinop::LESS;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGN LSEQ expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $4;
		binop->op = AstBinop::LSEQ;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGN GREATER expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $4;
		binop->op = AstBinop::GREATER;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGN GREQ expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $4;
		binop->op = AstBinop::GREQ;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGNMINUS expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $3;
		binop->op = AstBinop::MINUS;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGN PLUS expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $4;
		binop->op = AstBinop::PLUS;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGN PERCENT expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $4;
		binop->op = AstBinop::PERCENT;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGNMUL expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $3;
		binop->op = AstBinop::MUL;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGN DIV expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $4;
		binop->op = AstBinop::DIV;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGN BITSHL expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $4;
		binop->op = AstBinop::BITSHL;

		node->src = binop;
		$$ = node;
	} | expression_bitor ASSIGN BITSHR expression_assign {
		mknode(AstAssign, node, @1);
		node->dest = $1;

		mknode(AstBinop, binop, @1);
		binop->left = $1;
		binop->right = $4;
		binop->op = AstBinop::BITSHR;

		node->src = binop;
		$$ = node;
	}
	;

expression_bitor:
	expression_bitand {
		$$ = $1;
	} | expression_bitor BITOR expression_bitand {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::BITOR;
		$$ = node;
	}
	;

expression_bitand:
	expression_equality {
		$$ = $1;
	} | expression_bitand BITAND expression_equality {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::BITAND;
		$$ = node;
	}
	;

expression_equality:
	expression_compare {
		$$ = $1;
	} | expression_equality EQUAL expression_compare {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::EQUAL;
		$$ = node;
	} | expression_equality NEQUAL expression_compare {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::NEQUAL;
		$$ = node;
	}
	;

expression_compare:
	expression_bitshift { 
		$$ = $1;
	} | expression_compare GREATER expression_bitshift {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::GREATER;
		$$ = node;
	} | expression_compare LESS expression_bitshift {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::LESS;
		$$ = node;
	} | expression_compare GREQ expression_bitshift {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::GREQ;
		$$ = node;
	} | expression_compare LSEQ expression_bitshift {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::LSEQ;
		$$ = node;
	}
	;

expression_bitshift:
	expression_additive {
		$$ = $1;
	} | expression_bitshift BITSHL expression_additive {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::BITSHL;
		$$ = node;
	} | expression_bitshift BITSHR expression_additive {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::BITSHR;
		$$ = node;
	}
	;

expression_additive:
	expression_multiplicative {
		$$ = $1;
	} | expression_additive PLUS expression_multiplicative {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::PLUS;
		$$ = node;
	} | expression_additive MINUS expression_multiplicative {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::MINUS;
		$$ = node;
	}
	;

expression_multiplicative:
	expression_unary {
		$$ = $1;
	} | expression_multiplicative MUL expression_unary {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::MUL;
		$$ = node;
	} | expression_multiplicative DIV expression_unary {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::DIV;
		$$ = node;
	} | expression_multiplicative PERCENT expression_unary {
		mknode(AstBinop, node, @1);
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::PERCENT;
		$$ = node;
	}
	;

expression_unary:
	expression_postfix {
		$$ = $1;
	} | MUL expression_unary {
		mknode(AstDeref, node, @1);
		node->expression = $2;
		$$ = node;
	} | EXCLAMATION expression_unary {
		mknode(AstUnot, node, @1);
		node->expression = $2;
		$$ = node;
	} | MINUS expression_unary {
		mknode(AstUinv, node, @1);
		node->expression = $2;
		$$ = node;
	} | BITAND expression_unary {
		mknode(AstAddrof, node, @1);
		node->expression = $2;
		$$ = node;
	} | PLUSPLUS expression_postfix {
		mknode(AstIncDec, node, @1);
		node->expression = $2;
		node->type = AstIncDec::PRE;
		node->op = AstIncDec::INC;
		$$ = node;
	} | MINUSMINUS expression_postfix {
		mknode(AstIncDec, node, @1);
		node->expression = $2;
		node->type = AstIncDec::PRE;
		node->op = AstIncDec::DEC;
		$$ = node;
	}
	;

expression_postfix:
	expression_primary {
		$$ = $1;
	} | expression_postfix LBRACKET expression RBRACKET {
		mknode(AstIndex, node, @1);
		node->expression = $1;
		node->index = $3;
		$$ = node;
	} | expression_postfix LPAREN expression_list RPAREN {
		mknode(AstFuncCall, node, @1);
		node->expression = $1;
		node->args = $3;
		$$ = node;
	} | expression_postfix LPAREN RPAREN {
		mknode(AstFuncCall, node, @1);
		node->expression = $1;
		$$ = node;
	} | expression_postfix PLUSPLUS {
		mknode(AstIncDec, node, @1);
		node->expression = $1;
		node->type = AstIncDec::POST;
		node->op = AstIncDec::INC;
		$$ = node;
	} | expression_postfix MINUSMINUS {
		mknode(AstIncDec, node, @1);
		node->expression = $1;
		node->type = AstIncDec::POST;
		node->op = AstIncDec::DEC;
		$$ = node;
	}
	;

expression_primary:
	IDENTIFIER {
		mknode(AstVarRef, node, @1);
		node->name = $1;
		$$ = node;
	} | NUMBER {
		mknode(AstNumber, node, @1);
		node->number = $1;
		$$ = node;
	} | STRING_LIT {
		mknode(AstStringLit, node, @1);
		node->str = $1;
		$$ = node;
	} | LPAREN expression RPAREN {
		$$ = $2;	
	}
	;

%%

namespace blang {
	void Parser::error(const Parser::location_type& loc, const std::string& msg){
		throw ParserException(loc, msg);
	}
}
