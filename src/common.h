#ifndef CRISP_COMMON_H
#define CRISP_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// Forward declaration of the crisp interpreter type.
typedef struct crisp_t crisp_t;

// Forward declaration of the value type
// Defined in "value.h"
typedef struct value_t value_t;

// expr_t is an alias for a value pointer.
typedef value_t* expr_t;

// Forward declaration of environment type
// Defined in "environment.h
typedef struct env_t env_t;

#endif
