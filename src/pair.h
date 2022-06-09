#ifndef __PAIR_H__
#define __PAIR_H__

typedef struct Pair {
    char *first, *second; /* Primer y segundo elemento del par */
} Pair;

Pair *Pair_new(char *first, char *second);
void Pair_print(Pair *pair);

#endif