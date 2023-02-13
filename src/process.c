#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <dirent.h>
#include <errno.h>

#include "../include/process.h"
#include "../include/util.h"

int get_frame_count(char* video_name) {
    int frame_count = 0;
    char* cmd = "ffprobe -v error -select_streams v:0 -count_packets -show_entries stream=nb_read_packets -of csv=p=0 ";
    char* result = (char*) malloc(strlen(cmd) + strlen(video_name) + 1);
    strcpy(result, cmd);
    strcat(result, video_name);

    frame_count = fast_atoi(exec(result));
    free(result);

    return frame_count; // becuase idk 
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
    free(result);

    return duration;
}

void get_frames(int frame_count, char* video_name) {

    DIR* frames_dir = opendir("frames");
    if (frames_dir) {
        closedir(frames_dir);
    } else if (ENOENT == errno) {
        char* r = exec("mkdir frames");

        char duration[(int)((ceil(log10(frame_count))+1)*sizeof(char))];
        sprintf(duration, "%f", (double) frame_count / get_duration(video_name));

        char* cmd_p1 = "ffmpeg -i ";
        char* cmd_p2 = " -filter:v fps=fps=";
        char* cmd_p3 = " ./frames/frame_%06d.bmp &>/dev/null";
        char* result = (char*) malloc(strlen(cmd_p1) + strlen(cmd_p2) + strlen(cmd_p3) + strlen(video_name) + 1);

        strcpy(result, cmd_p1);
        strcat(result, video_name);
        strcat(result, cmd_p2);
        strcat(result, duration);
        strcat(result, cmd_p3);
        
        char* frame_creation_output = exec(result);
        free(result);

    } else {
        printf("unknown error!\n");
    }
}