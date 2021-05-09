all: $(patsubst %.c,out/%, $(wildcard *.c))

clean:
	rm -f out/*

out/%: %.c
	mkdir -p out
	gcc -Wall $*.c -o out/$*
