all: $(patsubst src/%.cpp,out/%, $(wildcard src/*.cpp))

clean:
	rm -f out/*

out/%: src/%.cpp src/shared.hpp
	mkdir -p out
	g++ -Wall -O0 -pthread src/$*.cpp -o out/$*
