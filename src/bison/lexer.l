%option c++
%option yyclass="Lexer"
%option noyywrap nounput noinput nodefault

%{
#include "parser.tab.hh"
#include "bison/lexer.hh"

using namespace yy;
%}

%%

%{
loc.step();
}%

[0-9]+ {
	return Parser::make_NUMBER(loc, std::stoi(yytext));
}

"+" { 
	return Parser::make_PLUS(loc); 
}

"*" { 
	return Parser::make_STAR(loc); 
}

[ \t\n]+ {}

. { return Parser::make_YYUNDEF(loc); }

%%
