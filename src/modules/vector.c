#include "vector.h"

#define MIN_CAPACITY 32


struct vector {
    uint32_t size;
    uint32_t capacity;
    void **array;
    Destructor destr_func;
};


Vector *vector_create(Destructor destr_func) {
    Vector *vec = calloc(1, sizeof(*vec));
	vec->capacity = MIN_CAPACITY;
    vec->array = calloc(vec->capacity, sizeof(void*));
    vec->destr_func = destr_func;
    return vec;
}


uint32_t vector_size(Vector *vec) {
    return vec->size;
}


void vector_insert(Vector *vec, void *value) {
    if (vec->size + 1 == vec->capacity) {
        vec->capacity *= 2;
        vec->array = realloc(vec->array, vec->capacity * sizeof(void*));
    }
    vec->array[vec->size++] = value;
}


void *vector_get(Vector *vec, uint32_t index) {
    return index < vec->size ? vec->array[index] : NULL;
}


void vector_delete(Vector *vec, uint32_t index) {
	if (index >= vec->size) 
		return;

    if (vec->destr_func != NULL)
        vec->destr_func(vec->array[index]);
    
    memmove(
		&vec->array[index],
		&vec->array[index] + 1UL,
        (vec->size - index - 1UL) * sizeof(void*)
    );
    vec->size--;
}


void vector_reset(Vector *vec) {
    for (size_t i = 0UL;  i < vec->size; ++i) {
        if (vec->destr_func != NULL)
            vec->destr_func(vec->array[i]);
    }
	vec->size = 0;

    if (vec->capacity > MIN_CAPACITY) {
	    vec->array = realloc(vec->array, MIN_CAPACITY * sizeof(void*));
        vec->capacity = MIN_CAPACITY;
    }
}


void vector_destroy(Vector *vec) {
    while (vec->size > 0)
        vector_delete(vec, vec->size - 1);

    free(vec->array);
    free(vec);
}


bool vector_find(Vector *vec, void *value, Comperator cmp_func, uint32_t *index) {
    for (size_t i = 0UL; i < vec->size; ++i) {
        if (cmp_func(value, vec->array[i]) == 0) {
            if (index != NULL) 
                *index = i;
            return true;
        }
    }
    return false;
}


