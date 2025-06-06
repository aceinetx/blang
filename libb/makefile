B = $$(find .. -name blang)

all: libb.a brt.o

libb.a: libb.o
	ar rcs $@ $<

libb.o: libb.b
	$(B) $< -o $@ -c

brt.o: brt.b
	$(B) $< -o $@ -c
