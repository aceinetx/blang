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
#include "ast/ast.hpp"
#include "frontend/driver.hpp"
#include <dep_pch.hpp>
namespace blang { 
	class Driver; 
}
}

%parse-param { blang::Driver& driver }
%lex-param { blang::Driver& driver }

%code {
	namespace blang {
		static Parser::symbol_type yylex(Driver& driver);
	}
}
%code requires {
}

%token END 0 "end of file"
%token <long> NUMBER
%token <std::string> IDENTIFIER
%token <std::string> STRING_LITERAL
%token LPAREN RPAREN LSQUIRLY RSQUIRLY SEMICOLON COMMA PLUS MINUS MULTIPLY DIVIDE AMPERSAND ASSIGN
%token RETURN ASM

%%

program:
	{
		driver.root = std::make_unique<blang::AstRootNode>();
	}
	;

%%

namespace blang {
	Parser::symbol_type yylex(Driver& driver){
		Parser::location_type loc (nullptr, 0, 0);
		if(driver.tokenIndex >= driver.tokens.size()) return Parser::make_END(loc);
		Token token = driver.tokens[driver.tokenIndex];
		++driver.tokenIndex;
		switch(token.type){
			case TokenType::Number:
				return Parser::make_NUMBER(std::get<long>(token.value.value()), loc);
			case TokenType::Identifier:
				return Parser::make_IDENTIFIER(std::get<std::string>(token.value.value()), loc);
			case TokenType::StringLiteral:
				return Parser::make_STRING_LITERAL(std::get<std::string>(token.value.value()), loc);
			case TokenType::Lparen:
				return Parser::make_LPAREN(loc);
			case TokenType::Rparen:
				return Parser::make_RPAREN(loc);
			case TokenType::Lsquirly:
				return Parser::make_LSQUIRLY(loc);
			case TokenType::Rsquirly:
				return Parser::make_RSQUIRLY(loc);
			case TokenType::Semicolon:
				return Parser::make_SEMICOLON(loc);
			case TokenType::Comma:
				return Parser::make_COMMA(loc);
			case TokenType::Add:
				return Parser::make_PLUS(loc);
			case TokenType::Sub:
				return Parser::make_MINUS(loc);
			case TokenType::Mul:
				return Parser::make_MULTIPLY(loc);
			case TokenType::Div:
				return Parser::make_DIVIDE(loc);
			case TokenType::Ampersand:
				return Parser::make_AMPERSAND(loc);
			case TokenType::Assign:
				return Parser::make_ASSIGN(loc);
			case TokenType::Return:
				return Parser::make_RETURN(loc);
			default:
				std::abort();
		}
	}

	void Parser::error(const Parser::location_type& loc, const std::string& msg){
		driver.error = msg;
	}
} // namespace blang
