#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>

#include "../include/render.h"
#include "../include/bmp.h"
#include "../include/util.h"

const char* GREYSCALE = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";

void compile_video(RenderSettings* opts, char** output) {

    int max_frame_width = 0;
    char* frame_str = (char*) malloc((int) (((opts->win_height/opts->scale) * (opts->win_width/opts->scale)) + opts->win_height/opts->scale));
    char* path = "frame_";
    char* ext = ".bmp";
    char* joined = (char*) malloc(5 + strlen(path) + strlen(ext));

    for (int f = 0; f < 400; f++) {
        Frame frame;

        int frame_num_len = (int)(ceil(log10(f+2)) * sizeof(char));
        char frame_num[frame_num_len];
        sprintf(frame_num, "%d", f+1);

        strcpy(joined, (*opts).frames_folder);
        strcat(joined, path);

        for (int i = 0; i < 6-frame_num_len; i++) {
            strcat(joined, "0");
        }

        strcat(joined, frame_num);
        strcat(joined, ext);

        int did_set = read_frame(&frame, joined);
        int rendered_frame_width = build_frame(&frame, opts, frame_str);

        if (rendered_frame_width > max_frame_width) {
            max_frame_width = rendered_frame_width;
        }

        printf("\rcompiled frame: %d", f + 1);
        fflush(stdout);

        strcpy(output[f], frame_str);
        strcat(output[f], "\0");
        
        if (did_set) {
            free(frame.pixel_data);
        }
        memset(joined, 0, strlen(joined));
        memset(frame_str, 0, (opts->win_width + 1) * opts->win_height);
    }

    free(joined);
    free(frame_str);
}

int build_frame(Frame* frame, RenderSettings* opts, char* output) {
    int p_i = 0;
    int r_x = frame->width / opts->win_width;
    int r_y = frame->height / opts->win_height;

    strcpy(output, "\0");

    for (int j = 0; j < frame->height; j += frame->height / (opts->win_height/opts->scale)) {
        int tp_i = 0;
        for (int i = 0; i < frame->width; i += frame->width / (opts->win_width/opts->scale)) {
            int luminance = sample_region(frame, r_x, r_y, i, j);
            
            int gscale_val = floor((255 - luminance) / (strlen(GREYSCALE) - 1));
            char pixel[2];
            pixel[0] = GREYSCALE[gscale_val];
            pixel[1] = 0;

            strcat(output, pixel);

            tp_i++;
        }

        if (tp_i > p_i) {
            p_i = tp_i;
        }

        strcat(output, "\n");
    }

    return p_i;
}

void render_video(RenderSettings* opts, char** frame_buffer) {
    clock_t start, end;
    start = clock();
    system("clear");
    printf("%s", frame_buffer[0]);
    end = clock();

    double cpu_time = (((double) (end - start)) / CLOCKS_PER_SEC); // seconds per frame
    unsigned int delay = ((1.0 / opts->fps) - cpu_time) * 1000; // ms

    for (int i = 0; i < 400; i++) {
        system("clear");
        printf("%s", frame_buffer[i]);

        usleep(delay * 1000); // µs
    }
}