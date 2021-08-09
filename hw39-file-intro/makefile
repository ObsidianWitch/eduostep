all: $(patsubst src/%.cpp,out/%, $(wildcard src/*.cpp))

clean:
	rm -rf out/

out/%: src/%.cpp src/*.hpp
	mkdir -p out/
	g++ -Wall src/$*.cpp -o out/$*
