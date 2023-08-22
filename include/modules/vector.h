#ifndef VECTOR_H
#define VECTOR_H

#include "common_types.h"

typedef struct vector Vector;

Vector *vector_create(Destructor destr_func);

uint32_t vector_size(Vector *vec);

void vector_insert(Vector *vec, void *value);

void *vector_get(Vector *vec, uint32_t index);

void vector_delete(Vector *vec, uint32_t index);

void vector_destroy(Vector *vec);

void vector_reset(Vector *vec);

bool vector_find(Vector *vec, void *value, Comperator cmp_func, uint32_t *index);

#endif /* VECTOR_H */
