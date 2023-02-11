#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "../include/render.h"
#include "../include/bmp.h"

const char* GREYSCALE = " .:-=+*%#@";

void compile_video(RenderSettings* opts, char** output) {

    int max_frame_width = 0;
    for (int f = 0; f < opts->frame_count; f++) {
        Frame frame;
        
        char* path = "frame_";
        char* ext = ".bmp";

        int frame_num_len = (int)(ceil(log10(f+2)) * sizeof(char));
        char frame_num[frame_num_len];
        sprintf(frame_num, "%d", f+1);

        char* joined = (char*) malloc(5 + strlen(path) + strlen(ext));
        strcpy(joined, (*opts).frames_folder);
        strcat(joined, path);

        if (4 - frame_num_len == 3) {
            strcat(joined, "000");
        } else if (4 - frame_num_len == 2) {
            strcat(joined, "00");
        } else if (4 - frame_num_len == 1) {
            strcat(joined, "0");
        }

        strcat(joined, frame_num);
        strcat(joined, ext);
        
        int did_set = read_frame(&frame, joined);
        char* frame_str = (char*) malloc((opts->win_width + 1) * opts->win_height); // program crashes here randomly...
        int rendered_frame_width = build_frame(&frame, opts, (char*) frame_str);

        if (rendered_frame_width > max_frame_width) {
            max_frame_width = rendered_frame_width;
        }

        printf("\rcompiled frame: %d", f + 1);
        fflush(stdout);
        
        strcpy(output[f], (char*) frame_str);

        if (did_set) {
            free(frame.pixel_data);
        }
        free(joined);
        free(frame_str);
    }
}

int build_frame(Frame* frame, RenderSettings* opts, char* output) {
    int p_j = 0;
    int p_i = 0;
    int r_x = frame->width / opts->win_width;
    int r_y = frame->height / opts->win_height;
    int m_x = r_x / opts->scale;
    int m_y = r_y / opts->scale;

    strcpy(output, "\0");

    for (int j = 0; j < frame->height; j += m_y) {
        for (int i = 0; i < frame->width; i += m_x) {
            int avg_luminance = 0;
            int samples = 0;

            for (int y = 0; y < r_y; y++) {
                for (int x = 0; x < r_x; x++) {
                    unsigned int r;
                    unsigned int g;
                    unsigned int b;

                    pixel_at(frame, &r, &g, &b, i+x, j+y);

                    avg_luminance += (r + g + b) / 3;
                    samples++;
                }
            }

            int gscale_val = (avg_luminance / samples) / 10;
            char pixel[2];
            pixel[0] = GREYSCALE[gscale_val];
            pixel[1] = 0;

            strcat(output, pixel);

            p_i++;
        }

        strcat(output, "\n");
        p_j++;
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
    unsigned int delay = ((1 / opts->fps) - cpu_time) * 1000;

    for (int i = 0; i < opts->frame_count; i++) {
        system("clear");
        printf("%s", frame_buffer[i]);

        usleep(delay * 1000);
    }
}