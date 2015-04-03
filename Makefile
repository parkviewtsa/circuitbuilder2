debug: src/main.cpp
	mkdir out
	gcc src/main.cpp -o out/circuitbuilder

.PHONY: clean

clean:
	rm -rf out
