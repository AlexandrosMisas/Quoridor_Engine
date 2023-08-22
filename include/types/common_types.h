#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#define _GNU_SOURCE
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>


#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef void (*Destructor)(void *a);
typedef int  (*Comperator)(void *a, void *b);


#endif /* COMMON_TYPES_H */
