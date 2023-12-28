#ifndef CRISP_INTERPRETER_H
#define CRISP_INTERPRETER_H

#include "common.h"

typedef enum
{
    CRISP_ERROR_NONE = 0,
    CRISP_ERROR_SCAN,
    CRISP_ERROR_PARSE,
    CRISP_ERROR_EVAL,
} crisp_error_t;

typedef void (*error_handler_t)(crisp_t *, void *);

crisp_t *init_interpreter();
void free_interpreter(crisp_t *crisp);

void install_error_handler(crisp_t *crisp, error_handler_t handler, void *handler_state);

expr_t read(crisp_t *crisp, const char *source);
expr_t eval(crisp_t *crisp, expr_t node, env_t *env);
void repl(crisp_t *crisp);

#endif
