%define api.pure full
%lex-param { yyscan_t scanner }
%parse-param { void *scanner }
%parse-param { blang::Parser *ctx }
%define lr.type lalr

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
	long number;
	std::string *string;
	blang::AstNode* node;
	blang::AstNode* stmt;
	std::vector<blang::AstNode*>* top_stmt_list;

	std::vector<blang::AstNode*>* stmt_list;
	std::vector<blang::AstNode*>* func_list;
	std::vector<blang::AstNode*>* node_list;
	
	std::vector<std::string>* identifier_list;
	std::vector<blang::AstNode*>* rvalue_commalist;
}

%token <number> NUMBER
%token <string> IDENTIFIER
%token <string> STRING_LITERAL
%token RETURN AUTO EXTRN IF ELSE WHILE BREAK
%token INCREMENT DECREMENT
%token PLUSASSIGN MINUSASSIGN MULTASSIGN DIVASSIGN
%token ASSIGN EQUAL NEQUAL GREATER LESS GREQ LSEQ PLUS MINUS MULTIPLY DIVIDE
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET SEMICOLON EXCLAMATION COMMA

%type <node> top_statement 
%type <node_list> top_statements
%type <node> function_definition
%type <node> statement 
%type <node_list> statement_list 
%type <identifier_list> identifier_list 
%type <node> return extrn func_call declaration assignment
%type <node_list> rvalue_commalist
%type <node> rvalue rvalue_bitand rvalue_shift rvalue_eq rvalue_cmp rvalue_pm rvalue_term rvalue_factor_no_lvalue rvalue_factor
%type <node> lvalue lvalue_factor

/*
%left BITOR
%left BITAND
%left BITSHL BITSHR
%left EQUAL NEQUAL
%left GREATER LESS GREQ LSEQ
%left PLUS MINUS
%left MULTIPLY DIVIDE 
%precedence EXCLAMATION */

%left BITOR
%left BITAND
%left BITSHL BITSHR
%left EQUAL NEQUAL
%left GREATER LESS GREQ LSEQ
%left PLUS MINUS
%left MULTIPLY DIVIDE /* modulo */
%right ASSIGN
%precedence EXCLAMATION

%%

program:
	top_statements {
		PARSER->root = new blang::AstRootNode();
		for (const auto& func : *$1) {
			PARSER->root->children.push_back(func);
		}
		delete $1;
	} | {
		PARSER->root = new blang::AstRootNode();
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
	} | IDENTIFIER LPAREN identifier_list RPAREN statement {
		auto* func = new blang::AstFuncDef();
		func->name = *$1;
		func->args = *$3;
		func->children.push_back($5);
		delete $3;
		delete $1;
		$$ = func;
	} | IDENTIFIER LPAREN identifier_list RPAREN LBRACE RBRACE {
		auto* func = new blang::AstFuncDef();
		func->name = *$1;
		func->args = *$3;
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
	} | IDENTIFIER LPAREN RPAREN statement {
		auto* func = new blang::AstFuncDef();
		func->name = *$1;
		func->children.push_back($4);
		delete $1;
		$$ = func;
	} | IDENTIFIER LPAREN RPAREN LBRACE RBRACE {
		auto* func = new blang::AstFuncDef();
		func->name = *$1;
		delete $1;
		$$ = func;
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

top_statements:
	top_statement {
		$$ = new std::vector<blang::AstNode*>();
		$$->push_back($1);
	} | top_statements top_statement {
		$1->push_back($2);
		$$ = $1;
	}
	;

top_statement:
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
	;

statement:
	return
	| extrn
	| func_call SEMICOLON
	| declaration
	| assignment
	;

assignment:
	lvalue ASSIGN rvalue SEMICOLON {
		auto* assign = new blang::AstVarAssign();
		assign->lexpr = $1;
		assign->rexpr = $3;
		$$ = assign;
	}

declaration:
	AUTO identifier_list SEMICOLON {
		auto* decl = new blang::AstVarDeclare();
		decl->names = *$2;
		delete $2;
		$$ = decl;
	}
	;

rvalue_commalist:
	rvalue {
		$$ = new std::vector<blang::AstNode*>();
		$$->push_back($1);
	} | rvalue_commalist COMMA rvalue {
    $1->push_back($3);
    $$ = $1;
	}

func_call:
	IDENTIFIER LPAREN rvalue_commalist RPAREN {
		auto *node = new blang::AstFuncCall();
		node->args = *$3;
		node->name = *$1;
		delete $1;
		delete $3;
		$$ = node;
	}
	| IDENTIFIER LPAREN RPAREN {
		auto *node = new blang::AstFuncCall();
		node->name = *$1;
		delete $1;
		$$ = node;
	}

extrn:
	EXTRN identifier_list SEMICOLON {
		auto *node = new blang::AstExtrn();
		node->names = *$2;
		delete $2;
		$$ = node;
	}

return:
	RETURN rvalue SEMICOLON {
		auto *node = new blang::AstReturn();
		node->expr = $2;
		$$ = node;
	}

lvalue:
	lvalue_factor
	| MULTIPLY lvalue_factor {
		auto* node = new blang::AstDeref();
		node->expr = $2;
		node->times = 1;
		$$ = node;
	}
	| MULTIPLY rvalue_factor_no_lvalue {
		auto* node = new blang::AstDeref();
		node->expr = $2;
		node->times = 1;
		$$ = node;
	}

lvalue_factor:
	IDENTIFIER {
		auto* var = new blang::AstVarRef();
		var->name = *$1;
		delete $1;
		$$ = var;
	}

rvalue:
	rvalue_bitand
	| rvalue BITOR rvalue_bitand {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "bitor";
		$$ = op;
	}
	| EXCLAMATION rvalue_bitand {
		auto* op = new blang::AstUnot();
		op->value = $2;
		$$ = op;
	}
	| MINUS rvalue_bitand {
		auto* op = new blang::AstUrev();
		op->value = $2;
		$$ = op;
	}
	;

rvalue_bitand:
	rvalue_shift
	| rvalue_bitand BITAND rvalue_shift {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "bitand";
		$$ = op;
	}
	;

rvalue_shift:
	rvalue_eq
	| rvalue_shift BITSHL rvalue_eq {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "bitshl";
		$$ = op;
	}
	| rvalue_shift BITSHR rvalue_eq {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "bitshr";
		$$ = op;
	}
	;

rvalue_eq:
	rvalue_cmp
	| rvalue_eq EQUAL rvalue_cmp {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "equal";
		$$ = op;
	}
	| rvalue_eq NEQUAL rvalue_cmp {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "nequal";
		$$ = op;
	}
	;

rvalue_cmp:
	rvalue_pm
	| rvalue_cmp GREATER rvalue_pm {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "greater";
		$$ = op;
	}
	| rvalue_cmp LESS rvalue_pm {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "less";
		$$ = op;
	}
	| rvalue_cmp GREQ rvalue_pm {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "greq";
		$$ = op;
	}
	| rvalue_cmp LSEQ rvalue_pm {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "lseq";
		$$ = op;
	}
	;

rvalue_pm:
	rvalue_term
	| rvalue_pm PLUS rvalue_term {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "add";
		$$ = op;
	}
	| rvalue_pm MINUS rvalue_term {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "sub";
		$$ = op;
	}
	;

rvalue_term:
	rvalue_factor
	| rvalue_term MULTIPLY rvalue_factor {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "mul";
		$$ = op;
	}
	| rvalue_term DIVIDE rvalue_factor {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "div";
		$$ = op;
	}
	;

rvalue_factor_no_lvalue:
	NUMBER {
		$$ = new blang::AstNumber($1);
	}
	| STRING_LITERAL {
		auto* var = new blang::AstStrRef();
		var->str = *$1;
		delete $1;
		$$ = var;
	}
	| LPAREN rvalue RPAREN {
		$$ = $2;
	}
	;

rvalue_factor:
	rvalue_factor_no_lvalue
	| lvalue
	;

%%

void yyerror([[maybe_unused]] yyscan_t scanner,blang::Parser *ctx, const char *s) {
	ctx->error = fmt::format("line {}: {}", ctx->line_num, s);
}
