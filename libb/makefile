B = $$(find .. -name blang)

all: libb.a brt.o
install: all
	cp libb.a /usr/lib
	cp brt.o /usr/lib

libb.a: libb.o
	ar rcs $@ $<

libb.o: libb.b
	$(B) $< -o $@ -c

brt.o: brt.b
	$(B) $< -o $@ -c
