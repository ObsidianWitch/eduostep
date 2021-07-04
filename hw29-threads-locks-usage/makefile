all: $(patsubst %.cpp,out/%, $(wildcard *.cpp))

clean:
	rm -f out/*

out/%: %.cpp shared.hpp
	mkdir -p out
	g++ -Wall -O0 -pthread $*.cpp -o out/$*
