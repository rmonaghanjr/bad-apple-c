#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <stdint.h>
#include <math.h>

#include "../include/bmp.h"
#include "../include/render.h"
#include "../include/util.h"
#include "../include/process.h"

int main(int argc, char** argv) {
    int start_frame = 0;

    int frame_count = get_frame_count(argv[1]);
    int duration = get_duration(argv[1]);

    printf("frame_count  = %d\n", frame_count);
    printf("duration     = %d\n", duration);
    printf("fps          = %f\n", (double) frame_count / duration);

    clock_t start, end;
    double cpu_time;

    start = clock();
    get_frames(frame_count, argv[1]);
    end = clock();

    cpu_time = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("frame_p_time = %fs\n", cpu_time * 10000);

    struct winsize window_size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size);

    int col = window_size.ws_col;
    int row = window_size.ws_row;

    if (col == 0) {
        col = 250;
        row = 50;
    }

    RenderSettings opts;
    opts.fps = (float) frame_count / duration;
    opts.frame_count = frame_count - 1;
    opts.frames_folder = "./frames/";
    opts.scale = 2;
    opts.win_width = col;
    opts.win_height = row;

    printf("allocating frame space...\n");
    char** frame_buffer = (char**) malloc(frame_count * sizeof(char*));
    for (int i = 0; i < frame_count; i++) {
        frame_buffer[i] = (char*) malloc((int) (((opts.win_height/opts.scale) * (opts.win_width/opts.scale)) + opts.win_height/opts.scale));
    }
    printf("allocated!\nbuilding frames...\n");

    compile_video(&opts, frame_buffer);
    render_video(&opts, frame_buffer);

    printf("deallocating frame space...\n");
    for (int i = 0; i < frame_count; i++) {
        free(frame_buffer[i]);
    }
    free(frame_buffer);

    printf("deallocated!\n");

    return 0;
}