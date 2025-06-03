input;
input_size;

NUMBER;
PLUS;
MINUS;
MULT;
DIV;

token;
number;
op;
pos;

get_input(){
	extrn malloc, memset, fgets, stdin, strcspn;

	input = malloc(input_size);
	memset(input, 0, input_size);

	fgets(input, input_size, stdin);
	*(input + strcspn(input, "\n")) = 0;

	pos = input;
}

is_digit(c){
	if(c >= '0'){
		if(c <= '9'){
			return 1;
		}
	}
	return 0;
}

do_number(){
		
}

next(){
	extrn puts, printf;

	auto end;
	end = 0;

	number = 0;
	op = 0;
	token = 0;

	while(!end){
		if(pos >= input + input_size) end = 1;

		if(!end){
			printf("%c %ld\n", *pos, is_digit(*pos));
			if(is_digit(*pos)){
				puts("DIGIT");
				end = 1;
			}

			pos += 1;
		}
	}
}

main(argc, argv){
	extrn printf, free;

	input_size = 1024;

	NUMBER = 1;
	PLUS = 2;
	MINUS = 3;
	MULT = 4;
	DIV = 5;

	get_input();

	printf("%s\n", input);

	next();
	printf("%ld %ld %c\n", token, number, op);

	free(input);
}
