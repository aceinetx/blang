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
#include "frontend/ast/AstGoto.hh"
#include "frontend/ast/AstLabel.hh"
#include "frontend/ast/AstUnot.hh"
#include "frontend/ast/AstIfChain.hh"

namespace blang { class Driver; }
}

%parse-param { blang::Driver& driver }
%lex-param { blang::Driver& driver }

%code {
	#include "frontend/Driver.hh"
	#include "frontend/ParserException.hh"

	namespace blang {
		Parser::symbol_type yylex(Driver& driver);
	}
}

%token END 0 "end of file"
%token <long> NUMBER
%token <std::string> IDENTIFIER
%token <std::string> STRING_LIT
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COLON ASSIGN PLUS MINUS MUL DIV AMPERSAND COMMA EQUAL NEQUAL GREATER LESS GREQ LSEQ EXCLAMATION LBRACKET RBRACKET
%token RETURN AUTO EXTRN WHILE GOTO IF ELSE

%type <std::shared_ptr<blang::AstFuncDef>> function_definition
%type <std::shared_ptr<blang::AstNode>> statement
%type <std::vector<std::shared_ptr<blang::AstNode>>> statement_list
%type <std::shared_ptr<blang::AstNode>> top_statement
%type <std::vector<std::shared_ptr<blang::AstNode>>> top_statement_list
%type <std::shared_ptr<blang::AstReturn>> return
%type <std::shared_ptr<blang::AstAutoVar>> auto
%type <std::shared_ptr<blang::AstExtern>> extrn
%type <std::shared_ptr<blang::AstWhile>> while
%type <std::shared_ptr<blang::AstGoto>> goto
%type <std::shared_ptr<blang::AstLabel>> label
%type <std::shared_ptr<blang::AstIfChain>> if_chain
%type <std::shared_ptr<blang::AstIf>> if
%type <std::shared_ptr<blang::AstElseIf>> elseif
%type <std::shared_ptr<blang::AstElse>> else
%type <std::vector<std::shared_ptr<blang::AstNode>>> expression_list
%type <std::shared_ptr<blang::AstNode>> expression expression_assign expression_equality expression_compare expression_additive expression_multiplicative expression_unary expression_postfix expression_primary
%type <std::vector<std::string>> identifier_list

%right ASSIGN
%left EQUAL NEQUAL
%left GREATER LESS GREQ LSEQ
%left AMPERSAND
%left PLUS MINUS
%left MUL DIV
%precedence EXCLAMATION

%%

program:
	top_statement_list {
		auto root = std::make_shared<blang::AstRoot>();
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
	IDENTIFIER LPAREN identifier_list RPAREN LBRACE statement_list RBRACE {
		auto node = std::make_shared<blang::AstFuncDef>();
		node->name = $1;
		node->statements = $6;
		node->args = $3;
		$$ = node;
	} | IDENTIFIER LPAREN RPAREN LBRACE statement_list RBRACE {
		auto node = std::make_shared<blang::AstFuncDef>();
		node->name = $1;
		node->statements = $5;
		$$ = node;
	}
	;

statement:
	return {
		$$ = $1;
	} | auto {
		$$ = $1;
	} | extrn {
		$$ = $1;
	} | while {
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
		auto node = std::make_shared<blang::AstIfChain>();
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
	IF LPAREN expression RPAREN LBRACE statement_list RBRACE {
		auto node = std::make_shared<blang::AstIf>();
		node->expression = $3;
		node->body = $6;
		$$ = node;
	}
	;

elseif:
	ELSE IF LPAREN expression RPAREN LBRACE statement_list RBRACE {
		auto node = std::make_shared<blang::AstElseIf>();
		node->expression = $4;
		node->body = $7;
		$$ = node;
	}
	;

else:
	ELSE LBRACE statement_list RBRACE {
		auto node = std::make_shared<blang::AstElse>();
		node->body = $3;
		$$ = node;
	}
	;
	
return:
	RETURN LPAREN expression RPAREN SEMICOLON {
		auto node = std::make_shared<blang::AstReturn>();
		node->expression = $3;
		$$ = node;
	}
	;

extrn:
	EXTRN identifier_list SEMICOLON {
		auto node = std::make_shared<blang::AstExtern>();
		node->names = std::move($2);
		$$ = node;
	}
	;

auto:
	AUTO IDENTIFIER SEMICOLON {
		auto node = std::make_shared<blang::AstAutoVar>();
		node->name = $2;
		$$ = node;
	}
	;

while:
	WHILE LPAREN expression RPAREN LBRACE statement_list RBRACE {
		auto node = std::make_shared<AstWhile>();
		node->expression = $3;
		node->statements = $6;
		$$ = node;
	}
	;

goto:
	GOTO IDENTIFIER SEMICOLON {
		auto node = std::make_shared<AstGoto>();
		node->name = $2;
		$$ = node;
	}
	;

label:
	IDENTIFIER COLON {
		auto node = std::make_shared<AstLabel>();
		node->name = $1;
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
	expression_equality {
		$$ = $1;
	} | expression_equality ASSIGN expression_assign {
		auto node = std::make_shared<blang::AstAssign>();
		node->dest = $1;
		node->src = $3;
		$$ = node;
	}
	;

expression_equality:
	expression_compare {
		$$ = $1;
	} | expression_equality EQUAL expression_compare {
		auto node = std::make_shared<AstBinop>();
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::EQUAL;
		$$ = node;
	} | expression_equality NEQUAL expression_compare {
		auto node = std::make_shared<AstBinop>();
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::NEQUAL;
		$$ = node;
	}
	;

expression_compare:
	expression_additive { 
		$$ = $1;
	} | expression_compare GREATER expression_additive {
		auto node = std::make_shared<AstBinop>();
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::GREATER;
		$$ = node;
	} | expression_compare LESS expression_additive {
		auto node = std::make_shared<AstBinop>();
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::LESS;
		$$ = node;
	} | expression_compare GREQ expression_additive {
		auto node = std::make_shared<AstBinop>();
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::GREQ;
		$$ = node;
	} | expression_compare LSEQ expression_additive {
		auto node = std::make_shared<AstBinop>();
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::LSEQ;
		$$ = node;
	}
	;

expression_additive:
	expression_multiplicative {
		$$ = $1;
	} | expression_additive PLUS expression_multiplicative {
		auto node = std::make_shared<AstBinop>();
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::PLUS;
		$$ = node;
	} | expression_additive MINUS expression_multiplicative {
		auto node = std::make_shared<AstBinop>();
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
		auto node = std::make_shared<AstBinop>();
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::MUL;
		$$ = node;
	} | expression_multiplicative DIV expression_unary {
		auto node = std::make_shared<AstBinop>();
		node->left = $1;
		node->right = $3;
		node->op = AstBinop::DIV;
		$$ = node;
	}
	;

expression_unary:
	expression_postfix {
		$$ = $1;
	} | MUL expression_unary {
		auto node = std::make_shared<blang::AstDeref>();
		node->expression = $2;
		$$ = node;
	} | EXCLAMATION expression_unary {
		auto node = std::make_shared<blang::AstUnot>();
		node->expression = $2;
		$$ = node;
	} | AMPERSAND expression_unary {
		auto node = std::make_shared<blang::AstAddrof>();
		node->expression = $2;
		$$ = node;
	}
	;

expression_postfix:
	expression_primary {
		$$ = $1;
	} | expression_postfix LBRACKET expression RBRACKET {
		auto node = std::make_shared<blang::AstIndex>();
		node->expression = $1;
		node->index = $3;
		$$ = node;
	} | expression_postfix LPAREN expression_list RPAREN {
		auto node = std::make_shared<blang::AstFuncCall>();
		node->expression = $1;
		node->args = $3;
		$$ = node;
	} | expression_postfix LPAREN RPAREN {
		auto node = std::make_shared<blang::AstFuncCall>();
		node->expression = $1;
		$$ = node;
	}
	;

expression_primary:
	IDENTIFIER {
		auto node = std::make_shared<blang::AstVarRef>();
		node->name = $1;
		$$ = node;
	} | NUMBER {
		auto node = std::make_shared<blang::AstNumber>();
		node->number = $1;
		$$ = node;
	} | STRING_LIT {
		auto node = std::make_shared<blang::AstStringLit>();
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
