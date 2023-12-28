#ifndef CRISP_INTERPRETER_INTERNAL_H
#define CRISP_INTERPRETER_INTERNAL_H

#include "interpreter.h"
#include "gc_type.h"

// Internal API functions for the crisp interpreter.

env_t *root_env(crisp_t *crisp);

const char *intern_string(crisp_t *crisp, const char *str, size_t length);
const char *intern_string_null_terminated(crisp_t *crisp, const char *str);

// Signals that an error has occurred
// Call flow will jump to the recovery position.
void crisp_error_jump(crisp_t *crisp, crisp_error_t err);

// Garbage collection functions.
void crisp_gc_register_object(crisp_t *crisp, gc_object_t* obj, gc_fn_t* fns);
void crisp_gc(crisp_t *crisp);

#endif //CRISP_INTERPRETER_INTERNAL_H
