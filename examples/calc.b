input;
input_size;

NUMBER;
PLUS;
MINUS;
MULT;
DIV;
LPAREN;
RPAREN;

token;
number;
pos;

die(){
	extrn free, exit;
	free(input);
	exit(1);
}

syntax(){
	extrn puts;
	if(token != 0){
		puts("syntax error");
		die();
	}
}

get_input(){
	extrn malloc, memset, fgets, stdin, strcspn, lchar;

	input = malloc(input_size);
	memset(input, 0, input_size);

	fgets(input, input_size, stdin);
	lchar(input, strcspn(input, "*n"), 0);

	pos = input;
}

is_digit(c){
	c =& 255;
	if(c >= '0'){
		if(c <= '9'){
			return(1);
		}
	}
	return(0);
}

is_term_op(token){
	if(token == MULT) return(1);
	if(token == DIV) return(1);
	return(0);
}

is_expr_op(token){
	if(token == PLUS) return(1);
	if(token == MINUS) return(1);
	return(0);
}

do_number(){
	auto end;
	while(1){
		if(pos >= input + input_size) break;

		if(is_digit(*pos)){
			number =* 10;
			number =+ (*pos & 255) - '0';
		} else break;

		pos =+ 1;
	}
}

next(){
	extrn puts, printf;

	auto end, ch;
	end = 0;

	number = 0;
	token = 0;

	while(1){
		if(pos >= input + input_size) break;
		if((*pos & 255) == 0) break;

		if(is_digit(*pos)){
			do_number();
			break;
		}

		ch = *pos & 255;
		if(ch == '+'){
			token = PLUS;
		} else if(ch == '-'){
			token = MINUS;
		} else if(ch == '*'){
			token = MULT;
		} else if(ch == '/'){
			token = DIV;
		} else if(ch == '('){
			token = LPAREN;
		} else if(ch == ')'){
			token = RPAREN;
		}

		pos =+ 1;
		if(token != 0) break;
	}
}

lit(){
	extrn puts;
	auto v;

	if(token != NUMBER)
		syntax();

	v = number;
	next();
	return(v);
}

factor(){
	extrn puts, expr, printf;

	if(token == LPAREN){
		auto v;
		next();
		
		v = expr();

		if(token != RPAREN)
			syntax();

		next();
		return(v);
	}
	return(lit());
}

term(){
	auto left, right, op;

	left = factor();
	
	op = token;
	while(is_term_op(op)){
		next();

		right = factor();

		if(op == MULT){
			left =* right;
		} else if(op == DIV){
			left =/ right;
		}
		op = token;
		if(op == 0) 
			break;
	}
	return(left);
}

expr(){
	auto left, right, op;

	left = term();
	
	op = token;
	while(is_expr_op(op)){
		next();

		right = term();

		if(op == PLUS){
			left =+ right;
		} else if(op == MINUS){
			left =- right;
		}
		op = token;
		if(op == 0) 
			break;
	}
	return(left);
}

main(argc, argv){
	extrn printf, free;

	input_size = 1024;

	NUMBER = 1;
	PLUS = 2;
	MINUS = 3;
	MULT = 4;
	DIV = 5;
	LPAREN = 6;
	RPAREN = 7;

	get_input();

	next();

	printf("%ld*n", expr());

	free(input);
}
