// originally from https://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file
// adapted to C by Ryan Monaghan

#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

typedef struct {
    unsigned char bmp_file_header[14];
    unsigned int pixel_array_offset;
    unsigned char bmp_info_header[40];

    int height;
    int width;
    int bits_per_pixel;

    int row_size;
    int pixel_array_size;

    unsigned char *pixel_data;
    const char *filename;
} BitMap;

void read_bmp(BitMap* result, char* filename) {

    FILE* fp;
    fp = fopen(filename, "r");

    fseek(fp, 0L, SEEK_END);

    int size = ftell(fp);
    char buff[size];

    fseek(fp, 0, SEEK_SET);

    if (!fp)
    {
        printf("cannot open file!\n");
    }

    unsigned char a;
    for (int i = 0; i < 14; i++)
    {
        (*result).bmp_file_header[i] = fgetc(fp);
    }

    if ((*result).bmp_file_header[0] != 'B' || (*result).bmp_file_header[1] != 'M')
    {
        printf("invalid header file!\n");
    }

    unsigned int *array_offset_ptr = (unsigned int *)((*result).bmp_file_header + 10);
    (*result).pixel_array_offset = *array_offset_ptr;

    if ((*result).bmp_file_header[11] != 0 || (*result).bmp_file_header[12] != 0 || (*result).bmp_file_header[13] != 0)
    {
        printf("unknown error!\n");
    }

    for (int i = 0; i < 40; i++)
    {
        (*result).bmp_info_header[i] = (int) fgetc(fp);
    }

    int *width_ptr = (int *)((*result).bmp_info_header + 4);
    int *height_ptr = (int *)((*result).bmp_info_header + 8);

    (*result).width = *width_ptr;
    (*result).height = *height_ptr;

    printf("w:%d h:%d\n", (*result).width, (*result).height);

    (*result).bits_per_pixel = (*result).bmp_info_header[14];
    if ((*result).bits_per_pixel != 24)
    {
        printf("invalid file!\n");
    }
    int compressionMethod = (*result).bmp_info_header[16];
    if (compressionMethod != 0)
    {
        printf("unhandled compression type!\n");
    }

    (*result).row_size = (int) (floor(((*result).bits_per_pixel * (*result).width + 31.) / 32)) * 4;
    (*result).pixel_array_size = (*result).row_size * abs((*result).height);

    (*result).pixel_data = (unsigned char*) malloc(sizeof(char) * (*result).pixel_array_size);

    fseek(fp, (*result).pixel_array_offset, SEEK_SET);
    for (int i = 0; i < (*result).pixel_array_size; i++)
    {
        (*result).pixel_data[i] = (uint32_t) fgetc(fp);
    }
}

void pixel_at(BitMap *bmp, unsigned int** v, int x, int y) {
    if (x < (*bmp).width && y < (*bmp).height) {
        y = (*bmp).height - 1 - y;

        (*v)[0] = (unsigned int)((*bmp).pixel_data[(*bmp).row_size * y + 3 * x + 2]);
        (*v)[1] = (unsigned int)((*bmp).pixel_data[(*bmp).row_size * y + 3 * x + 1]);
        (*v)[2] = (unsigned int)((*bmp).pixel_data[(*bmp).row_size * y + 3 * x + 0]);
    } else {
        printf("bad index!\n");
    }
}
#endif