#ifndef CRISP_INTERPRETER_H
#define CRISP_INTERPRETER_H

#include "common.h"

crisp_t* init_interpreter();
void free_interpreter(crisp_t* crisp);

expr_t read(crisp_t* crisp, const char* source);
expr_t eval(crisp_t* crisp, expr_t node);

const char* intern_string(crisp_t* crisp, const char* str, size_t length);
const char* intern_string_null_terminated(crisp_t* crisp, const char* str);

#endif
