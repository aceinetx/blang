%define api.pure full
%lex-param { yyscan_t scanner }
%parse-param { void *scanner }
%parse-param { blang::Parser *ctx }

%{
#include <pch.hh>
#include <src/ast/ast.hh>
#include <parser.hh>

typedef void* yyscan_t;
int yylex(YYSTYPE *yylval_param, yyscan_t scanner);
void yyerror(yyscan_t scanner,blang::Parser *ctx, const char *msg);

#define PARSER (yyget_extra(scanner))
%}

%code requires {
#include <src/ast/ast.hh>

namespace blang {
	struct Parser {
		unsigned long line_num = 1;
		blang::AstRootNode* root = nullptr;
		std::string error = "";
	};
}

blang::Parser* yyget_extra(void*);

}

%union {
	int number;
	std::string *string;
	blang::AstNode* node;
	blang::AstNode* stmt;

	std::vector<blang::AstNode*>* top_stmt_list;

	std::vector<blang::AstNode*>* stmt_list;
	std::vector<blang::AstNode*>* func_list;
	
	std::vector<std::string>* identifier_list;
	std::vector<blang::AstNode*>* expression_commalist;
}

%token <number> NUMBER
%token <string> IDENTIFIER
%token <string> STRING_LITERAL
%token RETURN AUTO EXTRN
%token ASSIGN PLUS MINUS MULTIPLY DIVIDE
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON AMPERSAND COMMA

%type <node> program function_definition
%type <node> expression term factor
%type <node> statement declaration assignment return_statement func_call extrn
%type <stmt_list> statement_list
%type <node> topstatement
%type <top_stmt_list> topstatements
%type <identifier_list> identifier_list
%type <expression_commalist> expression_commalist 

%left PLUS MINUS
%left MULTIPLY DIVIDE

%%

program:
	topstatements {
		PARSER->root = new blang::AstRootNode();
		for (const auto& func : *$1) {
			PARSER->root->children.push_back(func);
		}
		delete $1;
	}
	;

function_definition:
	IDENTIFIER LPAREN identifier_list RPAREN LBRACE statement_list RBRACE {
		auto* func = new blang::AstFuncDef();
		func->name = *$1;
		func->args = *$3;
		for (const auto& stmt : *$6) {
			func->children.push_back(stmt);
		}
		delete $6;
		delete $3;
		delete $1;
		$$ = func;
	} | IDENTIFIER LPAREN RPAREN LBRACE statement_list RBRACE {
		auto* func = new blang::AstFuncDef();
		func->name = *$1;
		for (const auto& stmt : *$5) {
			func->children.push_back(stmt);
		}
		delete $5;
		delete $1;
		$$ = func;
	}
	;

topstatements:
	topstatement {
		$$ = new std::vector<blang::AstNode*>();
		$$->push_back($1);
	} | topstatements topstatement {
		$1->push_back($2);
		$$ = $1;
	}
	;

topstatement:
	function_definition
	;

statement_list:
	statement {
		$$ = new std::vector<blang::AstNode*>();
		$$->push_back($1);
	}
	| statement_list statement {
		$1->push_back($2);
		$$ = $1;
	}
	| {
		$$ = new std::vector<blang::AstNode*>();
	}
	;

statement:
	declaration
	| assignment SEMICOLON
	| return_statement
	| func_call SEMICOLON
	| extrn
	;

extrn:
	EXTRN identifier_list SEMICOLON {
		auto* node = new blang::AstExtrn();
		node->names = *$2;
		delete $2;
		$$ = node;
	}

func_call:
	IDENTIFIER LPAREN expression_commalist RPAREN {
		auto* node = new blang::AstFuncCall();
		node->args = *$3;
		node->name = *$1;
		delete $3;
		delete $1;
		$$ = node;
	} | IDENTIFIER LPAREN RPAREN {
		auto* node = new blang::AstFuncCall();
		node->name = *$1;
		delete $1;
		$$ = node;
	}

expression_commalist:
	expression {
		$$ = new std::vector<blang::AstNode*>();
		$$->push_back($1);
	} | expression_commalist COMMA expression {
    $1->push_back($3);
    $$ = $1;
	}

declaration:
	AUTO identifier_list SEMICOLON {
		auto* decl = new blang::AstVarDeclare();
		decl->names = *$2;
		delete $2;
		$$ = decl;
	}
	;

identifier_list:
	IDENTIFIER {
		$$ = new std::vector<std::string>();
		$$->push_back(*$1);
		delete $1;
	} | identifier_list COMMA IDENTIFIER {
    $1->push_back(*$3);
    $$ = $1;
		delete $3;
	}

assignment:
	IDENTIFIER ASSIGN expression {
		auto* assign = new blang::AstVarAssign();
		assign->name = *$1;
		assign->expr = $3;
		delete $1;
		$$ = assign;
	}
	;

return_statement:
	RETURN expression SEMICOLON {
		auto* ret = new blang::AstReturn();
		ret->expr = $2;
		$$ = ret;
	}
	| RETURN SEMICOLON {
		auto* ret = new blang::AstReturn();
		$$ = ret;
	}
	;

expression:
	term
	| expression PLUS term {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "add";
		$$ = op;
	}
	| expression MINUS term {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "sub";
		$$ = op;
	}
	;

term:
	factor
	| term MULTIPLY factor {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "mul";
		$$ = op;
	}
	| term DIVIDE factor {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "div";
		$$ = op;
	}
	;

factor:
	NUMBER {
		$$ = new blang::AstNumber($1);
	}
	| IDENTIFIER {
		auto* var = new blang::AstVarRef();
		var->name = *$1;
		delete $1;
		$$ = var;
	}
	| STRING_LITERAL {
		auto* var = new blang::AstStrRef();
		var->str = *$1;
		delete $1;
		$$ = var;
	}
	| func_call
	| assignment
	| LPAREN expression RPAREN {
		$$ = $2;
	}
	;

%%

void yyerror([[maybe_unused]] yyscan_t scanner,blang::Parser *ctx, const char *s) {
	ctx->error = fmt::format("error at line {}: {}", ctx->line_num, s);
}
