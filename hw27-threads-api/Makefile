all: $(patsubst %.c,out/%, $(wildcard *.c))

clean:
	rm -rf out/

out/%: %.c common_threads.h
	mkdir -p out/
	gcc -Wall -pthread -g $*.c -o out/$*
