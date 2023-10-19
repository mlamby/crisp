#ifndef CRISP_VALUE_SUPPORT_H
#define CRISP_VALUE_SUPPORT_H

#include "value.h"

typedef struct
{
  expr_t src;
  expr_t iter;
  size_t index;
} list_iter_t;

static inline bool not(expr_t value) { return is_nil(value); }
static inline bool pair(expr_t value) { return is_cons(value); }

bool is_list(expr_t value);
size_t length(expr_t value);

// List iteration functions
list_iter_t iter_list(crisp_t *crisp, expr_t lst);
// Will return null if no more items are in the list.
expr_t iter_next(list_iter_t *i);

#endif
