#ifndef CRISP_EVALUATOR_H
#define CRISP_EVALUATOR_H

#include "common.h"

expr_t crisp_eval(crisp_t* crisp, expr_t node, env_t* env);
expr_t crisp_eval_list(crisp_t* crisp, expr_t list_node, env_t* env);
void crisp_bind_env(crisp_t* crisp, env_t* env, expr_t keys, expr_t values);
void crisp_eval_error(crisp_t* crisp, const char* fmt, ...);

#define EVAL_ASSERT(crisp, expr, msg) \
  if(!expr) {                         \
    crisp_eval_error(crisp, msg);     \
  }

#endif
