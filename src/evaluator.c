#include "evaluator.h"
#include "value.h"
#include "value_support.h"
#include "interpreter.h"
#include "builtins.h"
#include "environment.h"

#include <stdarg.h>
#include <stdio.h>

static expr_t apply(crisp_t *crisp, expr_t operator, expr_t operands, env_t *env);
static expr_t resolve_atom(crisp_t *crisp, expr_t node, env_t *env);

expr_t crisp_eval(crisp_t *crisp, expr_t node, env_t *env)
{
  (void)crisp;

  if (node == NULL)
    return NULL;

  if (is_bool(node) || is_string(node) || is_number(node) || is_nil(node))
    return node;

  if (is_atom(node))
  {
    return resolve_atom(crisp, node, env);
  }

  if (pair(node))
  {
    if (is_list(node))
    {
      return apply(crisp, crisp_eval(crisp, car(node), env), cdr(node), env);
    }
    else
    {
      crisp_eval_error(crisp, "Invalid list");
      return NULL;
    }
  }

  return node;
}

expr_t crisp_eval_list(crisp_t *crisp, expr_t list_node, env_t *env)
{
  if(is_nil(list_node))
  {
    return nil_value();
  }

  return cons(
    crisp_eval(crisp, car(list_node), env),
    crisp_eval_list(crisp, cdr(list_node), env));
}

#ifndef _MSC_VER
// Tell CLANG and GCC that the call to vprintf below
// is made within a function that implements the behvaviour
// of printf. Otherwise passing a non string literal as the
// first argument of vprintf will raise a warning.
// See: https://stackoverflow.com/a/20167541
__attribute__((__format__ (__printf__, 2, 0)))
#endif
void crisp_eval_error(crisp_t* crisp, const char* fmt, ...)
{
  (void)crisp;
  printf("Eval Error: ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  printf("\n");
  crisp_error_jump(crisp, CRISP_ERROR_EVAL);
}

static expr_t apply(crisp_t *crisp, expr_t operator, expr_t operands, env_t *env)
{
  if (is_fn(operator))
  {
    return as_fn(operator)(crisp, operands, env);
  }

  crisp_eval_error(crisp, "Can not apply a non function");
  return NULL;
}

static expr_t resolve_atom(crisp_t *crisp, expr_t node, env_t *env)
{
  expr_t value;
  const char *name = as_atom(node);
  if (!env_get(env, name, &value))
  {
    dump_env(env);
    crisp_eval_error(crisp, "Failed to resolve atom: <%p>%s", (void*)name, name);
    return NULL;
  }
  return value;
}
