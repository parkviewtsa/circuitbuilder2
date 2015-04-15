renderer:
	gcc -fpermissive circuit-render-test.cpp circuit-render.cpp -I . \
	-L /usr/lib/x86_64-linux-gnu -lSDL2-2.0 -o out/renderer

renderer-clang:
	clang circuit-render-test.cpp circuit-render.cpp -I . \
	-L /usr/lib/x86_64-linux-gnu -lSDL2-2.0 -o out/renderer

.PHONY: clean

clean:
	rm -rf out/*
