#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "wc.h"

void count_chars_and_lines(char *path, int *chars, int *lines) {
    FILE *fp;
    int c;

    fp = fopen(path, "r");
    if (!fp) return;

    *chars = 0;
    *lines = 0;
    
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            (*lines)++;
        }
        (*chars)++;
    }

    fclose(fp);
}