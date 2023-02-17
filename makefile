CFILES=src/main.c src/bmp.c src/process.c src/render.c src/util.c
COMPILER=gcc
CDEBUG=-g
CFLAGS=-Wall
OUT=cmd-play
INPUT=input.mp4 8

all:
	rm -rf build 
	mkdir build
	$(COMPILER) $(CFLAGS) $(CFILES) -o ./build/$(OUT)

debug:
	$(COMPILER) $(CFLAGS) $(CDEBUG) $(CFILES) -o ./build/$(OUT)

run:
	make
	./build/$(OUT) ./input/$(INPUT)

clean:
	rm -rf ./frames
	rm -rf build 
	mkdir build
	clear