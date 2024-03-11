#ifndef QUEUE_H
#define QUEUE_H

#include "vector.h"


#define queue_create(type, __destructor) \
    vector_create(type, VEC_MIN_CAP, __destructor)


#define queue_destroy(__queue) \
    vector_destroy(__queue)


#define queue_size(__queue) \
    vector_size(__queue)


#define queue_head(__queue) \
    (__queue)[0]


#define queue_insert(__queue, __entry) \
    vector_insert(__queue, __entry)


#define queue_delete(__queue) \
    vector_delete(__queue, 0)


#define queue_back(__queue) \
    (__queue)[(vector_size(__queue)) - 1]


#endif /* QUEUE_H */

