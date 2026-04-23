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
#include "frontend/Driver.hh"
#include "Assert.hh"

#include "frontend/ast/AstRoot.hh"
#include "frontend/ast/AstFuncDef.hh"
#include "frontend/ast/AstReturn.hh"
#include "frontend/ast/AstNumber.hh"
#include "frontend/ast/AstAutoVar.hh"
#include "frontend/ast/AstVarRef.hh"
#include "frontend/ast/AstAssign.hh"
#include "frontend/ast/AstBinop.hh"
#include "frontend/ast/AstDerefRv.hh"
#include "frontend/ast/AstDerefLv.hh"
#include "frontend/ast/AstAddrof.hh"
#include "frontend/ast/AstStringLit.hh"
#include "frontend/ast/AstExtern.hh"
#include "frontend/ast/AstFuncCall.hh"
#include "frontend/ast/AstUnot.hh"
#include "frontend/ast/AstWhile.hh"
#include "frontend/ast/AstLabel.hh"
#include "frontend/ast/AstGoto.hh"
#include "frontend/ast/AstIf.hh"
#include "frontend/ast/AstElseIf.hh"
#include "frontend/ast/AstElse.hh"
#include "frontend/ast/AstIfChain.hh"
#include "frontend/ast/AstIndex.hh"

namespace blang { class Driver; }
}

%parse-param { blang::Driver& driver }
%lex-param { blang::Driver& driver }

%code {
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
%type <std::vector<std::shared_ptr<blang::AstNode>>> expression_list
%type <std::shared_ptr<blang::AstNode>> expression expression_assign expression_primary
%type <std::shared_ptr<blang::AstLabel>> label
%type <std::shared_ptr<blang::AstGoto>> goto
%type <std::shared_ptr<blang::AstIf>> if
%type <std::shared_ptr<blang::AstElseIf>> elseif
%type <std::shared_ptr<blang::AstElse>> else
%type <std::shared_ptr<blang::AstIfChain>> if_chain
%type <std::vector<std::string>> identifier_list

%left EQUAL NEQUAL
%left GREATER LESS GREQ LSEQ
%left AMPERSAND
%left PLUS MINUS
%left MUL DIV
%right ASSIGN
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
	} | extrn {
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
	} | label {
		$$ = $1;
	} | goto {
		$$ = $1;
	} | expression SEMICOLON {
		$$ = $1;
	} | if_chain {
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
	
return:
	RETURN LPAREN expression RPAREN SEMICOLON {
		auto node = std::make_shared<blang::AstReturn>();
		node->expression = $3;
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

extrn:
	EXTRN identifier_list SEMICOLON {
		auto node = std::make_shared<blang::AstExtern>();
		node->names = $2;
		$$ = node;
	}
	;

label:
	IDENTIFIER COLON {
		auto node = std::make_shared<blang::AstLabel>();
		node->name = $1;
		$$ = node;
	}
	;

goto:
	GOTO IDENTIFIER SEMICOLON {
		auto node = std::make_shared<blang::AstGoto>();
		node->name = $2;
		$$ = node;
	}
	;

// TODO: break
while:
	WHILE LPAREN expression RPAREN LBRACE statement_list RBRACE {
		auto node = std::make_shared<blang::AstWhile>();
		node->expression = $3;
		node->statements = $6;
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
	expression_primary {
		$$ = $1;
	} | expression_primary ASSIGN expression_assign {
		auto node = std::make_shared<blang::AstAssign>();
		node->lvalue = $1;
		node->rvalue = $3;
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
		node->rvalue = false;
		node->number = $1;
		$$ = node;
	}
	;

%%

namespace blang {
	void Parser::error(const Parser::location_type& loc, const std::string& msg){
		throw std::runtime_error(fmt::format("{} at line {} column {}", msg, loc.begin.line, loc.begin.column));
	}
}
