// originally from https://stackoverflow.com/questions/9296059/read-pixel-value-in-frame-file
// adapted to C by Ryan Monaghan

#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

typedef struct {
    unsigned char frame_file_header[14];
    unsigned int pixel_array_offset;
    unsigned char frame_info_header[40];

    int height;
    int width;
    int bits_per_pixel;

    int row_size;
    int pixel_array_size;

    unsigned char *pixel_data;
    const char *filename;
} Frame;

int read_frame(Frame* result, char* filename);
void pixel_at(Frame* frame, unsigned int* r, unsigned int* g, unsigned int* b, int x, int y);

#endif