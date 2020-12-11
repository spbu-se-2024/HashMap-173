#ifndef HASH_HASH_H
#define HASH_HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH_WORD 128

typedef struct hash_data {
    char *key;
    int count;
    struct hash_data *next;
} HASH_DATA;

typedef struct hash_table {
    HASH_DATA **entries;
} HASH_TABLE;

typedef int (*hash_func_)(char*, int);

int md5_hash(char *msg, int table_size);

#endif