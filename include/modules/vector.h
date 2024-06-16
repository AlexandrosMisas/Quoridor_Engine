#ifndef VECTOR_H
#define VECTOR_H

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


#define VEC_MIN_CAP 16


struct vector {
	uint32_t size;
	uint32_t cap;
    void (*destructor)(void *v);
	bool is_ptr;
};


#define vector_factor(__size, __cap) ({		    \
	uint32_t __mult = 1;					    \
	while ((__size) > ((__cap) << __mult++))	\
		;									    \
	((__cap) << __mult);						\
})


#define vector_set_size(vec, __size) \
	((&((struct vector*)(vec))[-1])->size = (__size))


#define vector_set_flag(vec, __flag) \
	((&((struct vector*)(vec))[-1])->is_ptr = (__flag))


#define vector_set_cap(vec, __cap) \
	((&((struct vector*)(vec))[-1])->cap = (__cap))


#define vector_set_destructor(vec, __destructor) \
	((&((struct vector*)((vec)))[-1])->destructor = (__destructor))


#define vector_size(vec) \
	((&((struct vector*)(vec))[-1])->size)


#define vector_cap(vec) \
	((&((struct vector*)(vec))[-1])->cap)


#define vector_flag(vec) \
	((&((struct vector*)(vec))[-1])->is_ptr)


#define vector_destructor(vec) \
	((&((struct vector*)((vec)))[-1])->destructor)


#define vector_create(type, __cap, __destructor, is_ptr) ({												\
	uint64_t bytes = sizeof(type) * (__cap) + sizeof(struct vector);	                				\
	char *vec = (char*)calloc(1, bytes) + sizeof(struct vector);			            				\
	vector_set_size((vec), 0);												            				\
	vector_set_cap((vec), (__cap));												            			\
    vector_set_destructor((vec), (__destructor));                                           			\
	vector_set_flag((vec), (is_ptr));																	\
	((type*)vec);															            				\
})





#define vector_resize(vec, __new_cap) {												\
	uint64_t new_size = sizeof(*vec) * (__new_cap) + sizeof(struct vector);     	\
	vector_set_cap(vec, __new_cap);													\
	void *__vec = (&((struct vector*)(vec))[-1]);									\
	vec = (void*)((char*)realloc(__vec, new_size) + sizeof(struct vector));			\
}


#define vector_insert(vec, __entry) {	    \
	uint32_t __size = vector_size(vec);	    \
	uint32_t __cap  = vector_cap(vec);	    \
	if (__size == __cap)				    \
		vector_resize(vec, (__cap << 1));	\
	(vec)[__size] = (__entry);			    \
	vector_set_size(vec, (__size + 1));	    \
}

#define delete_entry(__entry, __func, __flag) {			\
	if (__func != NULL) {								\
		if (!__flag) {									\
			__func(&__entry);							\
		} else {										\
			uint64_t addr;								\
			memcpy(&addr, &__entry, sizeof(addr));		\
			__func((void*)addr);						\
		}												\
	}													\
}


#define vector_delete(vec, __index) {												\
	if (0 <= (__index) && (__index) <= (vector_size(vec) - 1)) {					\
		delete_entry(																\
			(vec)[__index],															\
			(vector_destructor(vec)),												\
			(vector_flag(vec))														\
		);																			\
		memmove(																	\
			&(vec)[__index],														\
			&(vec)[__index] + 1UL,													\
			(vector_size(vec) - __index - 1UL) * sizeof(*vec)						\
		);																			\
		vector_set_size(vec, (vector_size(vec) - 1));								\
	}																				\
}


#define vector_print(vec, __func) {							\
	if ((__func) != NULL) {									\
		for (size_t i = 0UL; i < vector_size(vec); ++i) {	\
			bool is_ptr = vector_flag(vec);					\
			if (!is_ptr) {									\
				__func(&(vec)[i]);							\
			} else {										\
				uint64_t addr;								\
				memcpy(&addr, &(vec)[i], sizeof(addr));		\
				__func((void*)addr);						\
			}												\
		}													\
	}														\
}


#define vector_destroy(vec) {											\
	for (size_t i = 0UL; i < vector_size(vec); ++i)						\
		delete_entry(vec[i], vector_destructor(vec), vector_flag(vec));	\
																		\
		free((&((struct vector*)(vec))[-1]));							\
}



#define vector_find(vec, __cmp, __entry) ({												\
	size_t __index = 0UL;																\
	while (__index < vector_size(vec)													\
		&& __cmp((vec)[__index++], __entry) != 0)										\
		;																				\
	(__index > 0 && __cmp((vec)[__index - 1], __entry) == 0) ? __index - 1 : -1;		\
})


#define vector_append(vec, __array, __size) {										\
	uint32_t space_left = vector_cap(vec) - vector_size(vec);						\
	if (space_left < (__size))														\
		vector_resize(vec, vector_factor((__size), vector_cap(vec)));				\
	memcpy(vec + (vector_size(vec)), (__array), (__size) * sizeof(*vec));			\
	vector_set_size(vec, (vector_size(vec) + (__size)));							\
}


#define vector_reset(vec, __cap) {				\
	vector_set_size(vec, 0);					\
	uint32_t cap__ = (__cap) < VEC_MIN_CAP		\
		? VEC_MIN_CAP							\
		: (__cap);								\
	vector_resize(vec, cap__);					\
}


#endif /* VECTOR_H */
