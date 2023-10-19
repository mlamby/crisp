#ifndef CRISP_INTERPRETER_H
#define CRISP_INTERPRETER_H

#include "common.h"

typedef enum { 
    ERROR_NONE,
    ERROR_SCAN,
    ERROR_PARSE,
    ERROR_EVAL,
} crisp_error_t;

crisp_t* init_interpreter();
void free_interpreter(crisp_t* crisp);

expr_t read(crisp_t* crisp, const char* source);
expr_t eval(crisp_t* crisp, expr_t node, env_t* env);
void repl(crisp_t* crisp);

env_t* root_env(crisp_t* crisp);

const char* intern_string(crisp_t* crisp, const char* str, size_t length);
const char* intern_string_null_terminated(crisp_t* crisp, const char* str);

// Signals that an error has occurred
// Call flow will jump to the recovery position.
void crisp_error_jump(crisp_t* crisp, crisp_error_t err);

#endif
