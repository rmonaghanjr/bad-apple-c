#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>

#include "bmp.h"

const int BUFFER_SIZE = 256;

int fast_atoi(const char* str) {
    int val = 0;

    while (*str) {
        if (*str - '0' >= 0) {
            val *= 10;
            val += (*str++ - '0');
        } else {
            break;
        }
    }

    return val;
}

char* exec(char* cmd) {
    char buffer[BUFFER_SIZE];

    char* data;
    FILE* stream;

    stream = popen(cmd, "r");
    if (stream) {
        while (!feof(stream)) {
            if (fgets(buffer, BUFFER_SIZE, stream) != NULL) {
                data = buffer;
            }
        }
        pclose(stream);
    }

    return data;
}

int get_frame_count(char* video_name) {
    int frame_count = 0;
    char* cmd = "ffprobe -v error -select_streams v:0 -count_packets -show_entries stream=nb_read_packets -of csv=p=0 ";
    char* result = (char*) malloc(strlen(cmd) + strlen(video_name) + 1);
    strcpy(result, cmd);
    strcat(result, video_name);

    frame_count = fast_atoi(exec(result));

    return frame_count;
}

int get_duration(char* video_name) {
    int duration = 0;
    char* cmd_p1 = "ffmpeg -i ";
    char* cmd_p2 = " 2>&1 | grep \"Duration\"| cut -d ' ' -f 4 | sed s/,// | sed 's@\\..*@@g' | awk '{ split($1, A, \":\"); split(A[3], B, \".\"); print 3600*A[1] + 60*A[2] + B[1] }'";
    char* result = (char*) malloc(strlen(cmd_p1) + strlen(cmd_p2) + strlen(video_name) + 1);
    strcpy(result, cmd_p1);
    strcat(result, video_name);
    strcat(result, cmd_p2);
    
    duration = fast_atoi(exec(result));

    return duration;
}

void get_frames(int frame_count, char* video_name) {
    // frame_count / get_duration(video_name)

    char duration[(int)((ceil(log10(frame_count))+1)*sizeof(char))];
    sprintf(duration, "%f", (double) frame_count / get_duration(video_name));

    char* cmd_p1 = "mkdir frames &>/dev/null && ffmpeg -i ";
    char* cmd_p2 = " -filter:v fps=fps=";
    char* cmd_p3 = " ./frames/frame_%04d.bmp &>/dev/null";
    char* result = (char*) malloc(strlen(cmd_p1) + strlen(cmd_p2) + strlen(cmd_p3) + strlen(video_name) + 1);

    strcpy(result, cmd_p1);
    strcat(result, video_name);
    strcat(result, cmd_p2);
    strcat(result, duration);
    strcat(result, cmd_p3);

    exec(result);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("missing video name!\n");
        return 1;
    }

    int frame_count = get_frame_count(argv[1]);
    int duration = get_duration(argv[1]);

    printf("frame_count  = %d\n", frame_count);
    printf("fps          = %f\n", (double) frame_count / duration);
    printf("duration     = %d\n", get_duration(argv[1]));

    clock_t start, end;
    double cpu_time;

    start = clock();
    get_frames(frame_count, argv[1]);
    end = clock();

    cpu_time = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("frame_p_time = %fs\n", cpu_time * 10000);

    system("clear");

    struct winsize window_size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size);

    int col = window_size.ws_col;
    int row = window_size.ws_row;

    return 0;
}