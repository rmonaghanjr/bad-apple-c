#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <math.h>
#include <pthread.h>
#include <sys/ioctl.h>

#include "../include/render.h"
#include "../include/bmp.h"
#include "../include/util.h"
#include "../include/process.h"

const char* GREYSCALE = " .:-=+*#%@";

void play_video(VIDEO_SETTINGS* video) {
    int frame_count = get_frame_count((*video).filename);
    int duration = get_duration((*video).filename);

    if ((*video).verbose) {
        printf("frame_count  = %d\n", frame_count);
        printf("duration     = %d\n", duration);
        printf("fps          = %f\n", (double) frame_count / duration);
    }

    clock_t start, end;
    double cpu_time;
    if ((*video).verbose) {
        start = clock();
    }

    get_frames(frame_count, (*video).filename);

    if ((*video).verbose) {
        end = clock();
        cpu_time = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("frame_p_time = %fs\n", cpu_time * 10000);
    }

    struct winsize window_size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size);

    int col = window_size.ws_col;
    int row = window_size.ws_row;

    if ((*video).verbose) {
        printf("win_size     = %dx%d\n", col, row);
    }

    RENDER_SETTINGS opts;
    opts.fps = (float) frame_count / duration;
    opts.frame_count = frame_count - 1;
    opts.frames_folder = (*video).frames_folder;
    opts.scale = (*video).scale;
    opts.win_width = col;
    opts.win_height = row;
    opts.available_cores = (*video).available_cores;

    char** frame_buffer = (char**) malloc(frame_count * sizeof(char*));
    for (int i = 0; i < frame_count; i++) {
        if ((*video).verbose) {
            printf("\rallocating block %d...", i);
            fflush(stdout);
        }
        
        frame_buffer[i] = (char*) malloc(((int) (((opts.win_height/opts.scale) * (opts.win_width/opts.scale)) + opts.win_height/opts.scale + opts.win_width/opts.scale)) * 2);
    }
    if ((*video).verbose) {
        printf("allocated!\nbuilding frames...\n");
    }
    distribute_sectors(&opts, frame_buffer);
    render_video(&opts, frame_buffer);

    for (int i = 0; i < frame_count; i++) {
        if ((*video).verbose) {
            printf("\rdeallocating block %d...", i);
            fflush(stdout);
        }

        free(frame_buffer[i]);
    }
    free(frame_buffer);

    if ((*video).verbose) {
        printf("deallocated!\n");
    }
}

void* compile_sector(void* args) {

    F_BUILDER_ARGS* dr_args = (F_BUILDER_ARGS*) args;

    int start = dr_args->start;
    int end = dr_args->end;
    RENDER_SETTINGS* opts = dr_args->opts;
    char** output = dr_args->frame_buffer;

    int max_frame_width = 0;
    char* path = "frame_";
    char* ext = ".bmp";
    char* joined = (char*) malloc(5 + strlen(path) + strlen(ext));

    for (int f = start; f < end; f++) {
        FRAME frame;

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
        
        memset(output[f], 0, ((int) (((opts->win_height/opts->scale) * (opts->win_width/opts->scale)) + opts->win_height/opts->scale + opts->win_width/opts->scale)) * 2);

        int did_set = read_frame(&frame, joined);
        int rendered_frame_width = build_frame(&frame, opts, output[f]);

        if (rendered_frame_width > max_frame_width) {
            max_frame_width = rendered_frame_width;
        }

        strcat(output[f], "\0");
        
        if (did_set) {
            free(frame.pixel_data);
        }
        memset(joined, 0, strlen(joined));
    }

    free(joined);

    pthread_exit((void*) 0);
}

void distribute_sectors(RENDER_SETTINGS* opts, char** frame_buffer) {
    int chunk_size = floor((*opts).frame_count / (*opts).available_cores);
    int remaining_frames = (*opts).frame_count - (chunk_size * (*opts).available_cores); // we need to add these frames to the last thread in order to have continuity

    int p_assignment = 0;
    F_BUILDER_ARGS* args = (F_BUILDER_ARGS*) malloc(sizeof(F_BUILDER_ARGS) * (*opts).available_cores);
    pthread_t fb_threads[(*opts).available_cores];

    for (int i = 0; i < (*opts).available_cores; i++) {
        args[i].opts = opts;
        args[i].frame_buffer = frame_buffer;
        args[i].start = p_assignment;
        args[i].end = p_assignment + chunk_size;

        if (i + 1 == (*opts).available_cores) {
            args[i].end += remaining_frames;
        }

        pthread_create(&fb_threads[i], NULL, &compile_sector, (void*) &args[i]);

        p_assignment = args[i].end;
    }

    int* exit_codes = (int*) malloc(sizeof(int) * (*opts).available_cores);
    for (int i = 0; i < (*opts).available_cores; i++) {
        pthread_join(fb_threads[i], (void**) &exit_codes[i]);
    }

    free(exit_codes);
    free(args);
}

int build_frame(FRAME* frame, RENDER_SETTINGS* opts, char* output) {
    int p_i = 0;
    int r_x = frame->width / opts->win_width;
    int r_y = frame->height / opts->win_height;

    strcpy(output, "\0");

    for (int j = 0; j < frame->height; j += frame->height / (opts->win_height/opts->scale)) {
        int tp_i = 0;
        for (int i = 0; i < frame->width; i += frame->width / (opts->win_width/opts->scale)) {
            int luminance = sample_region(frame, r_x, r_y, i, j);

            int gscale_val = floor((luminance * ((strlen(GREYSCALE) - 1)))/255);
            char pixel[2];
            pixel[0] = GREYSCALE[gscale_val];
            pixel[1] = 0;

            if (tp_i < opts->win_width) {
                strcat(output, pixel);
            }

            tp_i++;
        }

        if (tp_i > p_i) {
            p_i = tp_i;
        }

        strcat(output, "\n");
    }

    return p_i;
}

void render_video(RENDER_SETTINGS* opts, char** frame_buffer) {
    clock_t start, end;
    start = clock();
    system("clear");
    write(1, *(frame_buffer), strlen(*(frame_buffer)));
    end = clock();

    double cpu_time;
    unsigned int delay;

    for (int i = 0; i < opts->frame_count; i++) {
        start = clock();
        system("clear");
        write(1, *(frame_buffer + i), strlen(*(frame_buffer + i)));
        end = clock();

        cpu_time = (((double) (end - start)) / CLOCKS_PER_SEC);
        delay = ((1.0 / opts->fps) - cpu_time) * 1000;

        usleep(delay * 1000); // µs
    }
}