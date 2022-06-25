#include<stdio.h>
#include<errno.h>

/**
 * Count the total number of characters in the file
 * that *f points to
 */
long count_characters(FILE *f) {
    long last_pos;

    /* Returns the position of the last element of the file */
    fseek(f, -1L, 2);
    last_pos = ftell(f);
    last_pos++;
    return last_pos;
}