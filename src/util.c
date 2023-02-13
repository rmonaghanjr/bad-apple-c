#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/util.h"

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
    while (!feof(stream)) {
        if (fgets(buffer, BUFFER_SIZE, stream) != NULL) {
            data = buffer;
        }
    }
    pclose(stream);

    return data;
}