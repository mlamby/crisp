#ifndef CRISP_MEMORY_C
#define CRISP_MEMORY_C

#include "common.h"

// Allocate a block of memory.
// Type should be a typename that will be passed to sizeof.
// Count is the number of elements to allocate.
#define ALLOCATE(type, count) \
    (type*)reallocate(NULL, 0, sizeof(type) * (count), __FILE__, __LINE__)

// Free an already allocated block of memory.
#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0, __FILE__, __LINE__)

// Free an array of allocated memory.
#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0, __FILE__, __LINE__)

// Reallocate is the single function used to allocate and free memory.
// Do not directly use this function; instead use the ALLOCATE and
// FREE* macros.
// Reallocations are not currently supported.
void* reallocate(
    void* pointer,
    size_t old_size,
    size_t new_size,
    const char* file,
    unsigned int line);

// Features for memory monitoring and debugging.
// When memory logging has been started, all mallocs and frees are logged.
// A list of all allocated memory is maintained so that at any point
// memory_dump() can be called to print the list of allocated memory.
// When the program exits a list of all allocated memory is printed
// to the log file.
// Note: when memory logging is enabled, the ALLOCATE and FREE functions
//       are not thread-safe.
void memory_install_logging(const char* file_name);

// Dump the list of allocated memory.
void memory_dump();

#endif
