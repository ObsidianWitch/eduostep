all: $(patsubst %.cpp,out/%, $(wildcard *.cpp))

clean:
	rm -f out/*

out/%: %.cpp
	mkdir -p out
	g++ -Wall -O0 $*.cpp -o out/$*
