all: $(patsubst %.c,out/%, $(wildcard q*.c))

clean:
	rm -f out/*

out/q%: q%.c common.h
	mkdir -p out
	gcc -Wall -Werror common.h q$*.c -o out/q$*
