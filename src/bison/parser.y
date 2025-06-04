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

%type <node> program function_definition global_declaration
%type <node> lvalue rvalue rvalue_bitand rvalue_shift rvalue_eq rvalue_cmp rvalue_pm rvalue_term rvalue_factor
%type <node> if elif else if_chain
%type <node> statement_no_if statement declaration assignment return_statement func_call extrn deref addrof while break
%type <node> plus_assign minus_assign mult_assign div_assign bitshl_assign bitshr_assign bitand_assign bitor_assign
%type <stmt_list> statement_list
%type <node> topstatement
%type <top_stmt_list> topstatements
%type <identifier_list> identifier_list
%type <rvalue_commalist> rvalue_commalist 

%left BITOR
%left BITAND
%left BITSHL BITSHR
%left EQUAL NEQUAL
%left GREATER LESS GREQ LSEQ
%left PLUS MINUS
%left MULTIPLY DIVIDE /* modulo */
%right EXCLAMATION

%%

program:
	topstatements {
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
	| global_declaration
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

statement_no_if:
	declaration
	| assignment SEMICOLON
	| return_statement
	| func_call SEMICOLON
	| extrn
	| while
	| plus_assign
	| minus_assign
	| mult_assign
	| div_assign
	| bitshl_assign
	| bitshr_assign
	| bitand_assign
	| bitor_assign
	| break
	;

statement:
	statement_no_if
	| if_chain
	;

global_declaration:
	IDENTIFIER SEMICOLON {
		auto* node = new blang::AstGvarDeclare();
		node->name = *$1;
		delete $1;
		$$ = node;
	}

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

while:
	WHILE LPAREN rvalue RPAREN LBRACE statement_list RBRACE {
		auto* node = new blang::AstWhile();
		node->expr = $3;
		node->body = *$6;
		delete $6;
		$$ = node;
	} | WHILE LPAREN rvalue RPAREN statement {
		auto* node = new blang::AstWhile();
		node->expr = $3;
		node->body.push_back($5);
		$$ = node;
	}
	| WHILE LPAREN rvalue RPAREN LBRACE RBRACE {
		auto* node = new blang::AstWhile();
		node->expr = $3;
		$$ = node;
	}

break:
	BREAK SEMICOLON {
		auto* node = new blang::AstBreak();
		$$ = node;
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
	} | IF LPAREN rvalue RPAREN statement_no_if {
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
	| ELSE IF LPAREN rvalue RPAREN statement_no_if {
		auto* node = new blang::AstElif();
		node->expr = $4;
		node->body.push_back($6);
		$$ = node;
	}

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
	| ELSE statement_no_if {
		auto* node = new blang::AstElse();
		node->body.push_back($2);
		$$ = node;
	}

extrn:
	EXTRN identifier_list SEMICOLON {
		auto* node = new blang::AstExtrn();
		node->names = *$2;
		delete $2;
		$$ = node;
	}

func_call:
	IDENTIFIER LPAREN rvalue_commalist RPAREN {
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

rvalue_commalist:
	rvalue {
		$$ = new std::vector<blang::AstNode*>();
		$$->push_back($1);
	} | rvalue_commalist COMMA rvalue {
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
	lvalue ASSIGN rvalue {
		auto* assign = new blang::AstVarAssign();
		assign->lexpr = $1;
		assign->rexpr = $3;
		$$ = assign;
	}
	;

addrof:
	BITAND lvalue {
		auto* node = new blang::AstAddrof();
		node->expr = $2;
		node->times = 1;
		$$ = node;
	}

deref:
	MULTIPLY lvalue {
		auto* node = new blang::AstDeref();
		node->expr = $2;
		node->times = 1;
		$$ = node;
	}
	| MULTIPLY LPAREN rvalue RPAREN {
		auto* node = new blang::AstDeref();
		node->expr = $3;
		node->times = 1;
		$$ = node;
	}

return_statement:
	RETURN rvalue SEMICOLON {
		auto* ret = new blang::AstReturn();
		ret->expr = $2;
		$$ = ret;
	}
	| RETURN SEMICOLON {
		auto* ret = new blang::AstReturn();
		$$ = ret;
	}
	;

lvalue:
	IDENTIFIER {
		auto* var = new blang::AstVarRef();
		var->name = *$1;
		delete $1;
		$$ = var;
	}
	| lvalue LBRACKET rvalue RBRACKET {
		auto* var = new blang::AstArrIndex();
		var->expr = $1;
		var->index = $3;
		$$ = var;
	}
	| deref
	| addrof

rvalue:
	rvalue_bitand
	| EXCLAMATION rvalue {
		auto* op = new blang::AstUnot();
		op->value = $2;
		$$ = op;
	}
	| MINUS rvalue {
		auto* op = new blang::AstUrev();
		op->value = $2;
		$$ = op;
	}
	| rvalue BITOR rvalue_bitand {
		auto* op = new blang::AstBinaryOp();
		op->left = $1;
		op->right = $3;
		op->op = "bitor";
		$$ = op;
	}
	;

rvalue_bitand:
	| rvalue_shift
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

rvalue_factor:
	NUMBER {
		$$ = new blang::AstNumber($1);
	}
	| STRING_LITERAL {
		auto* var = new blang::AstStrRef();
		var->str = *$1;
		delete $1;
		$$ = var;
	}
	| func_call
	| assignment
	| lvalue
	| LPAREN rvalue RPAREN {
		$$ = $2;
	}
	;

%%

void yyerror([[maybe_unused]] yyscan_t scanner,blang::Parser *ctx, const char *s) {
	ctx->error = fmt::format("line {}: {}", ctx->line_num, s);
}
