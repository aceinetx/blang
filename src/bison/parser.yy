%require "3.8"
%language "c++"

%define api.namespace {yy}
%define api.parser.class {Parser}
%define api.value.type variant
%define api.token.raw
%define api.token.constructor

%define parse.assert
%define parse.trace
%define parse.error detailed
%define parse.lac full

%locations

%code requires {
#include <memory>
#include "ast/ast.hh"

namespace yy {
	class Lexer;
}
}

%parse-param { yy::Lexer& lexer }
%parse-param { std::shared_ptr<blang::AstNode> root }

%code {
#include "bison/lexer.hh"

#define yylex lexer.yylex
}

%token <int> NUMBER
%token PLUS STAR

%%

program:
	PLUS {
	}

%%

void yy::Parser::error(const location_type& loc, const std::string& msg)
{
    std::cout << loc << ": " << msg << std::endl;
}
