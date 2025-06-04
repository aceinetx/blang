B = $$(find .. -name blang)

all: calc hello curses

calc: calc.b
	$(B) $< -o $@

hello: hello.b
	$(B) $< -o $@

curses: curses.b
	$(B) $< -o $@ -l ncurses
