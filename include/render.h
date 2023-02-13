#ifndef RENDER_H
#define RENDER_H

#include "bmp.h"

typedef struct {
    short int win_width;
    short int win_height;
    short int scale;
    int frame_count;
    char* frames_folder;
    float fps;
    int available_cores;
} RenderSettings;

void compile_video(RenderSettings* opts, char** output);
void build_sector(RenderSettings* opts, int start, int end, char** frame_buffer);
int build_frame(Frame* frame, RenderSettings* opts, char* output);
void render_video(RenderSettings* opts, char** frame_buffer);

#endif