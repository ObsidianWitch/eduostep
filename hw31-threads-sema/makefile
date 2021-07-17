all: $(patsubst src/%.cpp,out/%, $(wildcard src/*.cpp))

clean:
	rm -f out/*

out/%: src/%.cpp
	mkdir -p out
	g++ -std=c++20 -Wall -pthread src/$*.cpp -o out/$*
