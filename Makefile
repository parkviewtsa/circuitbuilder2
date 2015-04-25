ARCH := x86_64
PLATFORM := linux-gnu

CPP := clang -c
LD := clang
CPPFLAGS := -I .
LDFLAGS := -L /usr/lib/$(ARCH)-$(PLATFORM) -lSDL2-2.0

BUILD_DIR := out

RENDER_OBJS := \
	$(BUILD_DIR)/circuit-render-test.o \
	$(BUILD_DIR)/circuit-render.o

APP_FILES := app/main.cpp

all: build-dir renderer cb2

scratch: clean all

build-dir:
	@mkdir -pv $(BUILD_DIR)

cb2: $(APP_FILES)
	@cd app; qmake
	@make -C app
	@mv app/app out/cb2

renderer: $(RENDER_OBJS)
	@echo
	@echo "Linking executable: " $@
	@$(LD) $(LDFLAGS) -o $(BUILD_DIR)/renderer $(RENDER_OBJS)

$(BUILD_DIR)/%.o: %.cpp
	@echo
	@echo "    CPP    " $<
	@$(CPP) $(CPPFLAGS) -o $@ $<

renderer-gcc:
	gcc -fpermissive circuit-render-test.cpp circuit-render.cpp -I . \
	-L /usr/lib/x86_64-linux-gnu -lSDL2-2.0 -o out/renderer

clean:
	@rm -rvf $(BUILD_DIR) app/Makefile
	@echo; echo "Entire build directory removed."; echo
