all: $(patsubst %.c,out/%, $(wildcard q*.c))

clean:
	rm -f out/*

out/q%: q%.c
	mkdir -p out
	gcc -Wall -Werror q$*.c -o out/q$*
