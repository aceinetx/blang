/* Functions that are not implemented in the cstd, but are in the b standard library */

char(string, i){
	return *(string + i);
}

lchar(string, i, char){
	*(string + i) = char & 0xFF;
}

printn(n,b) {
	extrn putchar;
	auto a;

	if(a=n/b) /* assignment, not test for equality */
		printn(a, b); /* recursive */
	putchar(n%b + '0');
}
