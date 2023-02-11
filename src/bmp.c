#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

#include "../include/bmp.h"

int read_frame(Frame* result, char* filename) {
    FILE* fp;
    fp = fopen(filename, "r");

    if (!fp) {
        printf("cannot open file! %s\n", filename);
        return 1;
    }

    fseek(fp, 0, SEEK_END);

    int size = ftell(fp);
    char buff[size];

    fseek(fp, 0, SEEK_SET);

    unsigned char a;
    for (int i = 0; i < 14; i++) {
        (*result).frame_file_header[i] = fgetc(fp);
    }

    if ((*result).frame_file_header[0] != 'B' || (*result).frame_file_header[1] != 'M') {
        printf("invalid header file!\n");
        return 1;
    }

    unsigned int *array_offset_ptr = (unsigned int *)((*result).frame_file_header + 10);
    (*result).pixel_array_offset = *array_offset_ptr;

    if ((*result).frame_file_header[11] != 0 || (*result).frame_file_header[12] != 0 || (*result).frame_file_header[13] != 0) {
        printf("unknown error!\n");
        return 1;
    }

    for (int i = 0; i < 40; i++) {
        fseek(fp, (*result).pixel_array_offset, SEEK_CUR + sizeof(int));
        (*result).frame_info_header[i] = (int) fgetc(fp);
    }

    int *width_ptr = (int *)((*result).frame_info_header + 4);
    int *height_ptr = (int *)((*result).frame_info_header + 8);

    (*result).width = *width_ptr;
    (*result).height = *height_ptr;

    (*result).bits_per_pixel = (*result).frame_info_header[14];
    if ((*result).bits_per_pixel != 24) {
        printf("invalid file!\n");
    }

    int compressionMethod = (*result).frame_info_header[16];
    if (compressionMethod != 0) {
        printf("unhandled compression type!\n");
        return 1;
    }

    (*result).row_size = (int) (floor(((*result).bits_per_pixel * (*result).width + 31.) / 32)) * 4;
    (*result).pixel_array_size = (*result).row_size * abs((*result).height);

    (*result).pixel_data = (unsigned char*) malloc(sizeof(char) * (*result).pixel_array_size);

    fseek(fp, (*result).pixel_array_offset, SEEK_SET);
    for (int i = 0; i < (*result).pixel_array_size; i++) {
        fseek(fp, (*result).pixel_array_offset, SEEK_CUR + sizeof(uint32_t));
        (*result).pixel_data[i] = (uint32_t) fgetc(fp);
    }

    fclose(fp); // dumbass lmao

    return 0;
}

void pixel_at(Frame *frame, unsigned int* r, unsigned int* g, unsigned int* b, int x, int y) {
    if (x < (*frame).width && y < (*frame).height) {
        y = (*frame).height - 1 - y;

        (*r) = (unsigned int)((*frame).pixel_data[(*frame).row_size * y + 3 * x + 2]);
        (*g) = (unsigned int)((*frame).pixel_data[(*frame).row_size * y + 3 * x + 1]);
        (*b) = (unsigned int)((*frame).pixel_data[(*frame).row_size * y + 3 * x + 0]);
    }
}