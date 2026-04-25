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
#include "frontend/ast/AstIfChain.hh"
#include "frontend/ast/AstBlock.hh"

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

%token RETURN AUTO EXTRN WHILE BREAK GOTO IF ELSE /* keywords */

%type <std::shared_ptr<blang::AstFuncDef>> function_definition
%type <std::shared_ptr<blang::AstBlock>> block
%type <std::shared_ptr<blang::AstNode>> statement
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
%type <std::shared_ptr<blang::AstIfChain>> if_chain
%type <std::shared_ptr<blang::AstIf>> if
%type <std::shared_ptr<blang::AstElseIf>> elseif
%type <std::shared_ptr<blang::AstElse>> else
%type <std::vector<std::shared_ptr<blang::AstNode>>> expression_list
%type <std::shared_ptr<blang::AstNode>> expression expression_assign expression_bitor expression_bitand expression_equality expression_compare expression_bitshift expression_additive expression_multiplicative expression_unary expression_postfix expression_primary
%type <std::vector<std::string>> identifier_list

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

function_definition:
	IDENTIFIER LPAREN identifier_list RPAREN statement {
		mknode(AstFuncDef, node, @1);
		node->name = $1;
		node->node_block = $5;
		node->args = $3;
		$$ = node;
	} | IDENTIFIER LPAREN RPAREN statement {
		mknode(AstFuncDef, node, @1);
		node->name = $1;
		node->node_block = $4;
		$$ = node;
	}
	;

statement:
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
	} | if_chain {
		$$ = $1;
	} | expression SEMICOLON {
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
		$$.clear();
		$$.push_back($1);
	}
	| identifier_list COMMA IDENTIFIER {
		$1.push_back($3);
		$$ = $1;
	}
	;

if_chain:
	if {
		mknode(AstIfChain, node, @1);
		node->begin_if = $1;
		$$ = node;
	} | if_chain elseif {
		$1->elifs.push_back($2);
		$$ = $1;
	} | if_chain else {
		$1->end_else = $2;
		$$ = $1;
	}
	;

if:
	IF LPAREN expression RPAREN block {
		mknode(AstIf, node, @1);
		node->expression = $3;
		node->body = $5;
		$$ = node;
	}
	;

elseif:
	ELSE IF LPAREN expression RPAREN block {
		mknode(AstElseIf, node, @1);
		node->expression = $4;
		node->body = $6;
		$$ = node;
	}
	;

else:
	ELSE block {
		mknode(AstElse, node, @1);
		node->body = $2;
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
		node->names = std::move($2);
		$$ = node;
	}
	;

auto:
	AUTO IDENTIFIER SEMICOLON {
		mknode(blang::AstAutoVar, node, @1);
		node->identifier_location = @2;
		node->name = $2;
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
		mknode(blang::AstAssign, node, @1);
		node->dest = $1;
		node->src = $3;
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
		mknode(blang::AstDeref, node, @1);
		node->expression = $2;
		$$ = node;
	} | EXCLAMATION expression_unary {
		mknode(blang::AstUnot, node, @1);
		node->expression = $2;
		$$ = node;
	} | BITAND expression_unary {
		mknode(blang::AstAddrof, node, @1);
		node->expression = $2;
		$$ = node;
	}
	;

expression_postfix:
	expression_primary {
		$$ = $1;
	} | expression_postfix LBRACKET expression RBRACKET {
		mknode(blang::AstIndex, node, @1);
		node->expression = $1;
		node->index = $3;
		$$ = node;
	} | expression_postfix LPAREN expression_list RPAREN {
		mknode(blang::AstFuncCall, node, @1);
		node->expression = $1;
		node->args = $3;
		$$ = node;
	} | expression_postfix LPAREN RPAREN {
		mknode(blang::AstFuncCall, node, @1);
		node->expression = $1;
		$$ = node;
	}
	;

expression_primary:
	IDENTIFIER {
		mknode(blang::AstVarRef, node, @1);
		node->name = $1;
		$$ = node;
	} | NUMBER {
		mknode(blang::AstNumber, node, @1);
		node->number = $1;
		$$ = node;
	} | STRING_LIT {
		mknode(blang::AstStringLit, node, @1);
		node->str = $1;
		$$ = node;
	}
	;

%%

namespace blang {
	void Parser::error(const Parser::location_type& loc, const std::string& msg){
		throw ParserException(loc, msg);
	}
}
