#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/render.h"
#include "../include/util.h"

int main(int argc, char** argv) {
    char* usage_p1 = "Usage: ";
    char* usage_p2 = " <input file (.mp4)> [--verbose, --cores=<int>, --scale=<int>]";
    char* usage_msg = (char*) malloc(strlen(usage_p1) + strlen(usage_p2) + strlen(argv[0]) + 1);
    strcpy(usage_msg, usage_p1);
    strcat(usage_msg, argv[0]);
    strcat(usage_msg, usage_p2);

    if (argc == 1) {
        printf("%s\n", usage_msg);
        free(usage_msg);
        return 0;
    }

    VIDEO_SETTINGS video;
    video.filename = argv[1];
    video.verbose = 0;
    video.available_cores = 1;
    video.scale = 2;
    video.frames_folder="./frames/";

    for (int i = 1; i < argc; i++) {
        if (i == 1 && argv[i][1] == '-') {
            printf("%s\n", usage_msg);
            free(usage_msg);
            return 0;
        }

        if (strcmp(argv[i], "--verbose") == 0) {
            video.verbose = 1;
            continue;
        }

        char* comparator = (char*) malloc(16);
        memset(comparator, 0, 16);

        for (int j = 0; j < strlen(argv[i]); j++) {
            if (strcmp(comparator, "--scale") == 0) {
                char* val = get_opt_value(argv[i]);
                video.scale = fast_atoi(val);
                free(val);
                break;
            } else if (strcmp(comparator, "--cores") == 0) {
                char* val = get_opt_value(argv[i]);
                video.available_cores = fast_atoi(val);
                free(val);
                break;
            }

            comparator[j] = argv[i][j];
        }
        free(comparator);
    }
    
    play_video(&video);

    free(usage_msg);

    return 0;
}
