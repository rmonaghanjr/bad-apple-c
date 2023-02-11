CFILES=src/main.c src/bmp.c src/process.c src/render.c src/util.c
COMPILER=gcc
OUT=bad-apple-c
INPUT=input.mp4

all:
	$(COMPILER) $(CFILES) -o ./build/$(OUT)

clean:
	rm -rf ./frames
	rm ./build/$(OUT)
	clear