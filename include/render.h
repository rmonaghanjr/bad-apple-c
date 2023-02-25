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
} RENDER_SETTINGS;

typedef struct {
    RENDER_SETTINGS* opts;
    char** frame_buffer;
    int start;
    int end;
} F_BUILDER_ARGS;

typedef struct {
    char* filename;
    char* frames_folder;
    int available_cores;
    int verbose;
    int scale;
} VIDEO_SETTINGS;

void play_video(VIDEO_SETTINGS* video);
void distribute_sectors(RENDER_SETTINGS* opts, char** frame_buffer);
void* compile_sector(void* args);
int build_frame(FRAME* frame, RENDER_SETTINGS* opts, char* output);
void render_video(RENDER_SETTINGS* opts, char** frame_buffer);

#endif
