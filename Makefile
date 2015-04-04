renderer:
	gcc -fpermissive circuit-render-test.cpp -I . -L /usr/lib/x86_64-linux-gnu \
	-lSDL2-2.0 -o out/renderer

.PHONY: clean

clean:
	rm -rf out/*
