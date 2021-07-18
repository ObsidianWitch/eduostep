BINARIES = $(patsubst src/%.c, out/%, $(wildcard src/vector-*.c))
COMMON = $(wildcard src/*.h) src/main-common.c

all: $(BINARIES)

clean:
	rm -rf out/

out/%: src/%.c $(COMMON)
	mkdir -p out/
	gcc -Wall -pthread -O1 src/$*.c -o out/$*
