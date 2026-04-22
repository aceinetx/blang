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

#include "frontend/ast/AstRoot.hh"
#include "frontend/ast/AstFuncDef.hh"
#include "frontend/ast/AstReturn.hh"
#include "frontend/ast/AstNumber.hh"
#include "frontend/ast/AstAutoVar.hh"
#include "frontend/ast/AstVarRefLv.hh"
#include "frontend/ast/AstVarRefRv.hh"
#include "frontend/ast/AstAssign.hh"
#include "frontend/ast/AstBinop.hh"
#include "frontend/ast/AstDerefRv.hh"
#include "frontend/ast/AstDerefLv.hh"
#include "frontend/ast/AstAddrof.hh"

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
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON ASSIGN PLUS MINUS MUL DIV AMPERSAND
%token RETURN AUTO

%type <std::shared_ptr<blang::AstFuncDef>> function_definition
%type <std::shared_ptr<blang::AstNode>> statement
%type <std::vector<std::shared_ptr<blang::AstNode>>> statement_list
%type <std::shared_ptr<blang::AstReturn>> return
%type <std::shared_ptr<blang::AstAutoVar>> auto
%type <std::shared_ptr<blang::AstNode>> rvalue rvalue_pm rvalue_term rvalue_factor lvalue
%type <std::shared_ptr<blang::AstNode>> constant

%left AMPERSAND
%left PLUS MINUS
%left MUL DIV
%right ASSIGN

%%

program:
	function_definition {
		auto root = std::make_shared<blang::AstRoot>();
		root->children.push_back($1);
		driver.set_root(root);
	}
	;

function_definition:
	IDENTIFIER LPAREN RPAREN LBRACE statement_list RBRACE {
		auto node = std::make_shared<blang::AstFuncDef>();
		node->name = $1;
		node->statements = $5;
		$$ = node;
	}
	;

statement:
	return {
		$$ = $1;
	}
	| auto {
		$$ = $1;
	}
	| rvalue SEMICOLON {
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
	
return:
	RETURN LPAREN rvalue RPAREN SEMICOLON {
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

rvalue:
	rvalue_pm {
		$$ = $1;
	} | lvalue ASSIGN rvalue {
		auto node = std::make_shared<blang::AstAssign>();
		node->lvalue = $1;
		node->rvalue = $3;
		$$ = node;
	}
	;

rvalue_pm:
	rvalue_term {
		$$ = $1;
	} | rvalue_pm PLUS rvalue_term {
		auto node = std::make_shared<blang::AstBinop>();
		node->left = $1;
		node->right = $3;
		node->op = blang::AstBinop::PLUS;
		$$ = node;
	} | rvalue_pm PLUS rvalue_term {
		auto node = std::make_shared<blang::AstBinop>();
		node->left = $1;
		node->right = $3;
		node->op = blang::AstBinop::PLUS;
		$$ = node;
	}
	;

rvalue_term:
	rvalue_factor {
		$$ = $1;
	} | rvalue_term MUL rvalue_factor {
		auto node = std::make_shared<blang::AstBinop>();
		node->left = $1;
		node->right = $3;
		node->op = blang::AstBinop::MUL;
		$$ = node;
	} | rvalue_term DIV rvalue_factor {
		auto node = std::make_shared<blang::AstBinop>();
		node->left = $1;
		node->right = $3;
		node->op = blang::AstBinop::DIV;
		$$ = node;
	}

rvalue_factor:
	constant {
		$$ = $1;
	} | IDENTIFIER {
		auto node = std::make_shared<blang::AstVarRefRv>();
		node->name = $1;
		$$ = node;
	} | LPAREN rvalue RPAREN {
		$$ = $2;
	} | MUL rvalue_pm {
		auto node = std::make_shared<blang::AstDerefRv>();
		node->expression = $2;
		$$ = node;
	} | AMPERSAND lvalue {
		auto node = std::make_shared<blang::AstAddrof>();
		node->expression = $2;
		$$ = node;
	}
	;

constant:
	NUMBER {
		auto node = std::make_shared<blang::AstNumber>();
		node->number = $1;
		$$ = node;
	}
	;

lvalue:
	IDENTIFIER {
		auto node = std::make_shared<blang::AstVarRefLv>();
		node->name = $1;
		$$ = node;
	} | MUL rvalue_pm {
		auto node = std::make_shared<blang::AstDerefLv>();
		node->expression = $2;
		$$ = node;
	}
	;

%%

namespace blang {
	void Parser::error(const Parser::location_type& loc, const std::string& msg){
		throw std::runtime_error(fmt::format("{} at line {} column {}", msg, loc.begin.line, loc.begin.column));
	}
}
