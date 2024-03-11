#ifndef VECTOR_H
#define VECTOR_H

#include "common_types.h"


#define VEC_MIN_CAP 16


struct vector {
	uint32_t size;
	uint32_t cap;
    void (*destructor)(void *v);
};


#define vector_factor(__size, __cap) ({		    \
	uint32_t __mult = 1;					    \
	while ((__size) > ((__cap) << __mult++))	\
		;									    \
	((__cap) << __mult);						\
})


#define vector_set_size(vec, __size) \
	((&((struct vector*)(vec))[-1])->size = (__size))


#define vector_set_cap(vec, __cap) \
	((&((struct vector*)(vec))[-1])->cap = (__cap))


#define vector_set_destructor(vec, __destructor) \
	((&((struct vector*)((vec)))[-1])->destructor = (__destructor))


#define vector_size(vec) \
	((&((struct vector*)(vec))[-1])->size)


#define vector_cap(vec) \
	((&((struct vector*)(vec))[-1])->cap)


#define vector_destructor(vec) \
	((&((struct vector*)((vec)))[-1])->destructor)


#define vector_create(type, __cap, __destructor) ({											\
	uint64_t bytes = sizeof(type) * (__cap) + sizeof(struct vector);	                	\
	char *vec = (char*)calloc(1, bytes) + sizeof(struct vector);			            	\
	vector_set_size((vec), 0);												            	\
	vector_set_cap((vec), __cap);												            \
    vector_set_destructor((vec), __destructor);                                           	\
	((type*)vec);															            	\
})


#define vector_destroy_entries(vec) {					\
	void (*destr)(void *v) = vector_destructor((vec));	\
	if (destr != NULL) {								\
		uint32_t __size = vector_size((vec));			\
		for (size_t i = 0UL; i < __size; ++i)			\
			destr((vec)[i]);							\
	}													\
}


#define vector_destroy(vec) \
	free((&((struct vector*)(vec))[-1]))



#define vector_grow(vec, __new_cap) {												\
	uint64_t new_size = sizeof(*vec) * (__new_cap) + sizeof(struct vector);     	\
	vector_set_cap(vec, __new_cap);													\
	void *__vec = (&((struct vector*)(vec))[-1]);									\
	vec = (void*)((char*)realloc(__vec, new_size) + sizeof(struct vector));			\
}


#define vector_insert(vec, __entry) {	\
	uint32_t __size = vector_size(vec);	\
	uint32_t __cap  = vector_cap(vec);	\
	if (__size == __cap)				\
		vector_grow(vec, (__cap << 1));	\
	(vec)[__size] = (__entry);			\
	vector_set_size(vec, (__size + 1));	\
}


#define vector_delete(vec, __index) {												\
	if (0 <= (__index) && (__index) <= (vector_size(vec) - 1)) {					\
		memmove(																	\
			&(vec)[(__index)],														\
			&(vec)[(__index)] + 1UL,												\
			(vector_size((vec)) - (__index) - 1UL) * sizeof(*vec)					\
		);																			\
		vector_set_size(vec, (vector_size(vec) - 1));								\
	}																				\
}


#define vector_delete_entry(vec, __index) {							\
	if (0 <= (__index) && (__index) <= (vector_size(vec) - 1)) {	\
		void (*destr)(void *v) = vector_destructor(vec);			\
		if (destr != NULL)											\
			destr((vec)[i]);										\
		vector_delete(vec, __index);								\
	}																\
}


#define vector_find(vec, __cmp, __entry) ({							\
	size_t __index = 0UL;											\
	while (__index < vector_size(vec)								\
		&& __cmp((vec)[__index++], __entry) != 0)					\
		;															\
	__cmp((vec)[__index - 1], __entry) == 0 ? __index - 1 : -1;		\
})


#define vector_append(vec, __array, __size) {								\
	uint32_t space_left = vector_cap(vec) - vector_size(vec);				\
	if (space_left < (__size))												\
		vector_grow(vec, vector_factor(__size, vector_cap(vec)));			\
	memcpy(vec + (vector_size(vec)), (__array), (__size) * sizeof(*vec));	\
	vector_set_size(vec, (vector_size(vec) + __size));						\
}


#define vector_reset(vec, __cap) {				\
	vector_set_size(vec, 0);					\
	uint32_t cap__ = __cap < VEC_MIN_CAP		\
		? VEC_MIN_CAP							\
		: __cap;								\
	vector_grow(vec, cap__);					\
}


#endif /* VECTOR_H */
