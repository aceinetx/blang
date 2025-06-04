main(){
	extrn initscr, curs_set, clear, mvprintw, refresh, getch;
	auto k, x, y;

	initscr();
	curs_set(0);

	x = y = 1;

	while(1){
		clear();

		mvprintw(0, 0, "Hello, from B!");
		mvprintw(y, x, "@");

		refresh();

		k = getch();
		if(k == 'q') break;
		else if(k == 'w') y -= 1;
		else if(k == 's') y += 1;
		else if(k == 'a') x -= 1;
		else if(k == 'd') x += 1;
	}

	endwin();
}
