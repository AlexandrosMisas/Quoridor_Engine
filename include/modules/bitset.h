#ifndef BITSET_H
#define BITSET_H

#include "common_types.h"


#define bitset_mask(__bit) \
    (1 << ((__bit) % CHAR_BIT))


#define bitset_create(__size) \
    (calloc((__size) / CHAR_BIT + (((__size) % CHAR_BIT) != 0), CHAR_BIT))


#define bitset_set(__bitset, __bit) \
    ((__bitset)[(__bit / CHAR_BIT)] |= bitset_mask(__bit))


#define bitset_clear(__bitset, __bit) \
    ((__bitset)[(__bit / CHAR_BIT)] &= ~bitset_mask(__bit))


#define bitset_check(__bitset, __bit) \
    ((__bitset)[__bit / CHAR_BIT] & bitset_mask(__bit))

#define bitset_destroy(__bitset) \
    free((__bitset))


#endif /* BITSET_H */