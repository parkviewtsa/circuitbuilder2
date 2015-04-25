ARCH := x86_64
PLATFORM := linux-gnu

CPP := clang -c
LD := clang

CPPFLAGS := -I .

LDFLAGS := -L /usr/lib/$(ARCH)-$(PLATFORM) -lSDL2-2.0

RENDER_OBJS := circuit-render-test.o circuit-render.o

all: renderer

renderer: $(RENDER_OBJS)
	@echo "    LD     " $@
	@$(LD) $(LDFLAGS) -o out/renderer $(RENDER_OBJS)

%.o: %.cpp
	@echo "    CPP    " $<
	@$(CPP) $(CPPFLAGS) -o $@ $<

renderer-gcc:
	gcc -fpermissive circuit-render-test.cpp circuit-render.cpp -I . \
	-L /usr/lib/x86_64-linux-gnu -lSDL2-2.0 -o out/renderer

clean:
	@rm -rvf out/* *.o ./*/*.o
