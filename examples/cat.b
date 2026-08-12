main(argc, argv){
	extrn fopen,perror,fclose,fseek,SEEK_SET,SEEK_END,ftell,malloc,puts,fread;
	auto fp,sz,buf;

	if(argc != 2) {
		puts("filename");
		return(1);
	}
	fp = fopen(argv[1], "r");
	if(!fp) {
		perror("fopen");
		return(1);
	}
	if(fseek(fp, 0, SEEK_END)){
		perror("fseek");
		return(1);
	}
	sz = ftell(fp);
	if(fseek(fp, 0, SEEK_SET)){
		perror("fseek");
		return(1);
	}
	buf = malloc(sz + 1);
	if(fread(buf, 1, sz, fp) != sz){
		perror("fread");
		return(1);
	}
	buf[sz] = 0;
	puts(buf);
	fclose(fp);
	return(0);
}

SEEK_SET 0;
SEEK_END 2;
