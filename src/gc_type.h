#ifndef CRISP_GC_TYPE_H
#define CRISP_GC_TYPE_H

// This header is called from common.h so it needs to stand-alone.
// It provides definitions for GC related data types.

#include <stdbool.h>

typedef struct gc_object_t gc_object_t;

// Function pointer typedef for a function that can be used
// to release the memory of an object.
typedef void (*gc_fn_ptr)(gc_object_t *);

typedef struct {

  // The destructor function for this object.
  // Used to release the memory of the object.
  gc_fn_ptr free_fn;

  // The information function for this object.
  // Used to print information about the object.
  gc_fn_ptr info_fn;
} gc_fn_t;

// Denotes that the structure containing this type has it's memory
// managed by the Garbage Collector. 
// It should be the first member of the structure to allow casting
// between gc_object_t and the actual structure. For example:
//
//   struct my_managed_struct {
//     gc_object_t gc_object;
//     int x;
//   };
//
// All garbage collected objects are managed in a single-linked list.
// The head of the list is stored in the gc_object_t.next field.
//
struct gc_object_t
{
  // Next item in the linked list.
  gc_object_t* next;

  // Functions to perform operations on the object.
  gc_fn_t* functions;

  // Denotes that the object has been marked and therefore should
  // not be cleared when the sweep stage is run.
  bool marked;
};

#endif //CRISP_GC_TYPE_H