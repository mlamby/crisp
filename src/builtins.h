#ifndef CRISPY_BUILTIN_H
#define CRISPY_BUILTIN_H

#include "common.h"

void register_builtins(crisp_t* crisp);

expr_t b_is_list(crisp_t *crisp, expr_t operands);

#endif
