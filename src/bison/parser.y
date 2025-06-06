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
%type <node> function_definition global_declaration
%type <node> statement statement_no_cs
%type <node> if_chain if else elif
%type <node_list> statement_list 
%type <identifier_list> identifier_list 
%type <node> return extrn func_call declaration assignment while break
%type <node> plus_assign minus_assign mult_assign div_assign bitshl_assign bitshr_assign bitand_assign bitor_assign
%type <node_list> rvalue_commalist
%type <node> rvalue rvalue_bitand rvalue_shift rvalue_eq rvalue_cmp rvalue_pm rvalue_term rvalue_factor_no_lvalue rvalue_factor
%type <node> lvalue lvalue_factor

%left BITOR
%left BITAND
%left BITSHL BITSHR
%left EQUAL NEQUAL
%left GREATER LESS GREQ LSEQ
%left PLUS MINUS
%left MULTIPLY DIVIDE PERCENT
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
	| global_declaration
	;

global_declaration:
	IDENTIFIER SEMICOLON {
		auto* node = new blang::AstGvarDeclare();
		node->name = *$1;
		delete $1;
		$$ = node;
	}

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

statement_no_cs:
	return
	| extrn
	| func_call SEMICOLON
	| declaration
	| assignment
	| break
	| plus_assign
	| minus_assign
	| mult_assign
	| div_assign
	| bitshl_assign
	| bitshr_assign
	| bitand_assign
	| bitor_assign
	;

statement:
	statement_no_cs
	| if_chain
	| while
	;

bitshl_assign:
	lvalue BITSHL ASSIGN rvalue SEMICOLON {
		auto* assign = new blang::AstAssignBinop();
		assign->var = $1;
		assign->value = $4;
		assign->op = "bitshl";
		$$ = assign;
	}

bitshr_assign:
	lvalue BITSHR ASSIGN rvalue SEMICOLON {
		auto* assign = new blang::AstAssignBinop();
		assign->var = $1;
		assign->value = $4;
		assign->op = "bitshr";
		$$ = assign;
	}

bitand_assign:
	lvalue BITAND ASSIGN rvalue SEMICOLON {
		auto* assign = new blang::AstAssignBinop();
		assign->var = $1;
		assign->value = $4;
		assign->op = "bitand";
		$$ = assign;
	}

bitor_assign:
	lvalue BITOR ASSIGN rvalue SEMICOLON {
		auto* assign = new blang::AstAssignBinop();
		assign->var = $1;
		assign->value = $4;
		assign->op = "bitor";
		$$ = assign;
	}

plus_assign:
	lvalue PLUSASSIGN rvalue SEMICOLON {
		auto* assign = new blang::AstAssignBinop();
		assign->var = $1;
		assign->value = $3;
		assign->op = "add";
		$$ = assign;
	}

minus_assign:
	lvalue MINUSASSIGN rvalue SEMICOLON {
		auto* assign = new blang::AstAssignBinop();
		assign->var = $1;
		assign->value = $3;
		assign->op = "sub";
		$$ = assign;
	}

mult_assign:
	lvalue MULTASSIGN rvalue SEMICOLON {
		auto* assign = new blang::AstAssignBinop();
		assign->var = $1;
		assign->value = $3;
		assign->op = "mul";
		$$ = assign;
	}

div_assign:
	lvalue DIVASSIGN rvalue SEMICOLON {
		auto* assign = new blang::AstAssignBinop();
		assign->var = $1;
		assign->value = $3;
		assign->op = "div";
		$$ = assign;
	}

if_chain:
	if {
		auto* node = new blang::AstIfChain();
		node->ifs.push_back($1);
		$$ = node;
	}
	| if_chain elif {
		((blang::AstIfChain*)$1)->ifs.push_back($2);
		((blang::AstIfChain*)$$)->ifs = ((blang::AstIfChain*)$1)->ifs;
	}
	| if_chain else {
		((blang::AstIfChain*)$1)->ifs.push_back($2);
		((blang::AstIfChain*)$$)->ifs = ((blang::AstIfChain*)$1)->ifs;
	}
	;

if:
	IF LPAREN rvalue RPAREN LBRACE statement_list RBRACE {
		auto* node = new blang::AstIf();
		node->expr = $3;
		node->body = *$6;
		delete $6;
		$$ = node;
	} | IF LPAREN rvalue RPAREN statement_no_cs {
		auto* node = new blang::AstIf();
		node->expr = $3;
		node->body.push_back($5);
		$$ = node;
	}
	| IF LPAREN rvalue RPAREN LBRACE RBRACE {
		auto* node = new blang::AstIf();
		node->expr = $3;
		$$ = node;
	}
	;

elif:
	ELSE IF LPAREN rvalue RPAREN LBRACE statement_list RBRACE {
		auto* node = new blang::AstElif();
		node->expr = $4;
		node->body = *$7;
		delete $7;
		$$ = node;
	} 
	| ELSE IF LPAREN rvalue RPAREN LBRACE RBRACE {
		auto* node = new blang::AstElif();
		node->expr = $4;
		$$ = node;
	}
	| ELSE IF LPAREN rvalue RPAREN statement_no_cs {
		auto* node = new blang::AstElif();
		node->expr = $4;
		node->body.push_back($6);
		$$ = node;
	}
	;

else:
	ELSE LBRACE statement_list RBRACE {
		auto* node = new blang::AstElse();
		node->body = *$3;
		delete $3;
		$$ = node;
	}
	| ELSE LBRACE RBRACE {
		auto* node = new blang::AstElse();
		$$ = node;
	}
	| ELSE statement_no_cs {
		auto* node = new blang::AstElse();
		node->body.push_back($2);
		$$ = node;
	}
	;

while:
	WHILE LPAREN rvalue RPAREN LBRACE statement_list RBRACE {
		auto* node = new blang::AstWhile();
		node->expr = $3;
		node->body = *$6;
		delete $6;
		$$ = node;
	}
	| WHILE LPAREN rvalue RPAREN LBRACE RBRACE {
		auto* node = new blang::AstWhile();
		node->expr = $3;
		$$ = node;
	}
	| WHILE LPAREN rvalue RPAREN statement {
		auto* node = new blang::AstWhile();
		node->expr = $3;
		node->body.push_back($5);
		$$ = node;
	}
	;

break:
	BREAK SEMICOLON {
		auto* node = new blang::AstBreak();
		$$ = node;
	}
	;

assignment:
	lvalue ASSIGN rvalue SEMICOLON {
		auto* assign = new blang::AstVarAssign();
		assign->lexpr = $1;
		assign->rexpr = $3;
		$$ = assign;
	}
	;

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
	| MULTIPLY lvalue {
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
	| rvalue_factor_no_lvalue LBRACKET rvalue RBRACKET {
		auto* var = new blang::AstArrIndex();
		var->expr = $1;
		var->index = $3;
		$$ = var;
	}

lvalue_factor:
	IDENTIFIER {
		auto* var = new blang::AstVarRef();
		var->name = *$1;
		delete $1;
		$$ = var;
	}
	| lvalue_factor LBRACKET rvalue RBRACKET {
		auto* var = new blang::AstArrIndex();
		var->expr = $1;
		var->index = $3;
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
	| MINUS rvalue_bitand {
		auto* op = new blang::AstUrev();
		op->value = $2;
		$$ = op;
	}
	| lvalue ASSIGN rvalue_bitand {
		auto* assign = new blang::AstVarAssign();
		assign->lexpr = $1;
		assign->rexpr = $3;
		$$ = assign;
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
	| rvalue_term PERCENT rvalue_factor {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "mod";
		$$ = op;
	}

	| EXCLAMATION rvalue_factor {
		auto* op = new blang::AstUnot();
		op->value = $2;
		$$ = op;
	}

	| INCREMENT rvalue_factor {
		auto* op = new blang::AstIncDec();
		op->expr = $2;
		op->type = blang::AstIncDec::PRE;
		op->op = blang::AstIncDec::INC;
		$$ = op;
	}
	| rvalue_factor INCREMENT {
		auto* op = new blang::AstIncDec();
		op->expr = $1;
		op->type = blang::AstIncDec::POST;
		op->op = blang::AstIncDec::INC;
		$$ = op;
	}
	| DECREMENT rvalue_factor {
		auto* op = new blang::AstIncDec();
		op->expr = $2;
		op->type = blang::AstIncDec::PRE;
		op->op = blang::AstIncDec::DEC;
		$$ = op;
	}
	| rvalue_factor DECREMENT {
		auto* op = new blang::AstIncDec();
		op->expr = $1;
		op->type = blang::AstIncDec::POST;
		op->op = blang::AstIncDec::DEC;
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
	| func_call
	;

rvalue_factor:
	rvalue_factor_no_lvalue
	| lvalue
	| BITAND lvalue {
		auto* node = new blang::AstAddrof();
		node->expr = $2;
		node->times = 1;
		$$ = node;
	}
	;

%%

void yyerror([[maybe_unused]] yyscan_t scanner,blang::Parser *ctx, const char *s) {
	ctx->error = fmt::format("line {}: {}", ctx->line_num, s);
}
