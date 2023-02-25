# project config
BUILD_DIR := build
EXECUTABLE := cmd-play
FRAMES_FOLDER := ./frames/
CORES := $(shell sysctl -n hw.logicalcpu)

# compiler config
COMPILER := clang
FLAGS := -c -g -Wall -Werror -pedantic-errors

# source files
RENDER_FILES := process.c render.c 
VIDEO_FILES := bmp.c
FILES := main.c util.c $(RENDER_FILES) $(VIDEO_FILES)
OBJECTS := $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(FILES)))

.PHONY: $(EXECUTABLE)
$(EXECUTABLE): $(BUILD_DIR)/$(EXECUTABLE)

.PRECIOUS: $(BUILD_DIR)/. $(BUILD_DIR)%/.

$(BUILD_DIR)/.:
	mkdir -p $@

$(BUILD_DIR)%/.:
	mkdir -p $@

.SECONDEXPANSION:

$(BUILD_DIR)/%.o: src/%.c | $$(@D)/.
	$(COMPILER) $(FLAGS) $< -o $@

$(BUILD_DIR)/$(EXECUTABLE): $(OBJECTS)
	$(COMPILER) $^ -o ./$(BUILD_DIR)/$(EXECUTABLE)

run:
	make
	./$(BUILD_DIR)/$(EXECUTABLE) ./input/input.mp4 --verbose --scale=1 --cores=$(CORES) --frames-folder=$(FRAMES_FOLDER)

new:
	make clean
	make

clean:
	rm -rf build
	mkdir build