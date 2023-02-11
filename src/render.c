#include <stdlib.h>
#include <stdio.h>

#include "../include/render.h"
#include "../include/bmp.h"

const char* GREYSCALE = " .:-=+*#%@";

void compile_video(RenderSettings* opts, char** output) {
    for (int f = 0; f < (*opts).frame_count; f++) {
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
        build_frame(&frame, opts, output);
        printf("\rcompiling frame: %d", f);
        fflush(stdout);

        if (!did_set) {
            free(frame.pixel_data);
        } else {
            break;
        }

        free(joined);
    }
}

void build_frame(Frame* frame, RenderSettings* opts, char* output) {
    system("clear");
    
    for (int j = 0; j < frame->height; j += frame->height / (opts->win_height/opts->scale)) {
        int y = j;

        for (int i = 0; i < frame->width; i += frame->width / (opts->win_width/opts->scale)) {
            int avg_luminance = 0;

            // need to allocate for 12 pixels of data
            // store values in an array and hard code it
            // only works for bad_apple given the sample ratio of 6 rows to 2 cols (12 pixels, 3 vals each, 36 total ints needed)

            unsigned int pixel_values[36];

            pixel_at(frame, &pixel_values[0], &pixel_values[1], &pixel_values[2], i+0, j+0);
            pixel_at(frame, &pixel_values[3], &pixel_values[4], &pixel_values[5], i+1, j+0);
            pixel_at(frame, &pixel_values[6], &pixel_values[7], &pixel_values[8], i+2, j+0);
            pixel_at(frame, &pixel_values[9], &pixel_values[10], &pixel_values[11], i+3, j+0);
            pixel_at(frame, &pixel_values[12], &pixel_values[13], &pixel_values[14], i+4, j+0);
            pixel_at(frame, &pixel_values[15], &pixel_values[16], &pixel_values[17], i+5, j+0);
            pixel_at(frame, &pixel_values[18], &pixel_values[19], &pixel_values[20], i+0, j+1);
            pixel_at(frame, &pixel_values[21], &pixel_values[22], &pixel_values[23], i+1, j+1);
            pixel_at(frame, &pixel_values[24], &pixel_values[25], &pixel_values[26], i+2, j+1);
            pixel_at(frame, &pixel_values[27], &pixel_values[28], &pixel_values[29], i+3, j+1);
            pixel_at(frame, &pixel_values[30], &pixel_values[31], &pixel_values[32], i+4, j+1);
            pixel_at(frame, &pixel_values[33], &pixel_values[34], &pixel_values[35], i+5, j+1);

            for (int s = 0; s < 36; s++) {
                avg_luminance += pixel_values[s];
            }
            avg_luminance /= 36;
            
            printf("%c", GREYSCALE[(int) (avg_luminance/11)]);
        }

        printf("\n");
    }
}

void render_video(RenderSettings* opts, char** frame_buffer) {
    for (int i = 0; i < (*opts).frame_count; i++) {
        printf("%s\n", frame_buffer[i]);
    }
}