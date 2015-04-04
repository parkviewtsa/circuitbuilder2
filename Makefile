renderer:
	gcc src/circuit-render.cpp src/circuit-render-test.cpp \
	src/circuit-render-proto.cpp -o out/renderer

.PHONY: clean

clean:
	rm -rf out/*
