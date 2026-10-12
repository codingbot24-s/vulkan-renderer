//
// Created by saad on 10/12/26.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log/loger.h"
#include <stdbool.h>

static bool is_space(const char c) {
    if (c == ' ') {
        return true;
    }

    return false;
}

/// return 1 if success else crash
/// out buffer the data if obj will be in that outbuffer
int read_obj_and_store_in_buffer(const char* filename, char** out_buffer) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        R_FATAL("Could not open model file");
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        R_FATAL("seek failed");

    }
    const ssize_t fsize = ftell(f);
    if (fsize < 0) {
        fclose(f);
        R_FATAL("ftell failed");
    }
    rewind(f);

    /// buffer that stores the obj data from file
    char *buffer = (char*) malloc(fsize + 1);
    if (buffer == NULL) {
        fclose(f);
        R_FATAL("buffer malloc failed");
    }

    size_t bytes_read = fread(buffer, 1, fsize, f);
    if (bytes_read < fsize) {
        free(buffer);
        fclose(f);
        R_FATAL("buffer malloc failed");
    }
    buffer[bytes_read] = '\0';

    *out_buffer = buffer;
    return 1;
}


int parse_line(char* line) {
    char *token = line;
    if (token[0] == '\0') {
        return 0;
    }
    if (token[0] == '#') {
        return 0;
    }
    /// this will x , y , z of vertex
    if (token[0] == 'v' && is_space(token[1])) {
        float x, y, z;
        if (sscanf(token, "v %f %f %f", &x, &y, &z) == 3) {

        }
    }
}

/// pass the filename (obj) for loading
void load_model(const char *filename) {
    char* buffer;
    if (read_obj_and_store_in_buffer(filename, &buffer) != 1) {
        return;
    }
    char* line = strtok(buffer, "\n");
    while (line != NULL) {
        parse_line(line);
        line = strtok(NULL, "\n");
    }
}