#ifndef CRISP_MEMORY_C
#define CRISP_MEMORY_C

#include "common.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#define ALLOCATE(type, count) \
    (type*)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

#endif
