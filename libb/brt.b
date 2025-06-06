_start(){
	extrn main, exit;
	auto ret;

	ret = main();
	exit(ret);	
}
