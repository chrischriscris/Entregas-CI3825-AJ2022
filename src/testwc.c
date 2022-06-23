#include <stdio.h>
#include "wc.h"

int main(int argc, char **argv) {
    int lines = 0;
    int chars = 0;
    count_chars_and_lines(argv[1], &chars, &lines);
    printf("%d %d\n", chars, lines);
    return 0;
}