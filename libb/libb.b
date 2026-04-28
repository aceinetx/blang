/* Functions that are not implemented in the cstd, but are in the b standard library */

char(string, i){
	return(*(string + i) & 255);
}

lchar(string, i, char){
	*(string + i) = char & 255;
}

printn(n,b) {
	extrn putchar;
	auto a;

	if(a=n/b) /* assignment, not test for equality */
		printn(a, b); /* recursive */
	putchar(*("0123456789ABCDEFGHJKLMNOP" + n%b));
);
}
