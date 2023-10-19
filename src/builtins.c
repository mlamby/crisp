#include "builtins.h"
#include "value.h"
#include "value_support.h"
#include "interpreter.h"
#include "environment.h"
#include "evaluator.h"

#define intern intern_string_null_terminated

#define CHECK_OPERAND(c, tst, op, msg)        \
  if (!tst)                                   \
  {                                           \
    printf("operand (");                      \
    print_value(op);                          \
    printf(") failed check: %s\n", msg);      \
    crisp_eval_error(crisp, "Check Operand"); \
    return NULL;                              \
  }

expr_t b_quote(crisp_t *crisp, expr_t operands, env_t *env)
{
  (void)crisp;
  (void)env;
  return car(operands);
}

static double operator_add(double a, double b) { return a + b; }
static double operator_sub(double a, double b) { return a - b; }
static double operator_mult(double a, double b) { return a * b; }
static double operator_div(double a, double b) { return a / b; }
typedef double (*binary_op_t)(double a, double b);

static expr_t b_binary_numerical(crisp_t *crisp, expr_t operands, env_t *env, binary_op_t op)
{
  bool first = true;
  double result = 0.0;
  list_iter_t iter = iter_list(crisp, crisp_eval_list(crisp, operands, env));
  expr_t operand = NULL;

  while ((operand = iter_next(&iter)))
  {
    CHECK_OPERAND(crisp, is_number(operand), operand, "Must be a number");
    if (first)
    {
      result = as_number(operand);
      first = false;
    }
    else
    {
      result = op(result, as_number(operand));
    }
  }
  return number_value(result);
}

static expr_t b_add(crisp_t *crisp, expr_t operands, env_t *env)
{
  return b_binary_numerical(crisp, operands, env, operator_add);
}

static expr_t b_sub(crisp_t *crisp, expr_t operands, env_t *env)
{
  return b_binary_numerical(crisp, operands, env, operator_sub);
}

static expr_t b_mult(crisp_t *crisp, expr_t operands, env_t *env)
{
  return b_binary_numerical(crisp, operands, env, operator_mult);
}

static expr_t b_div(crisp_t *crisp, expr_t operands, env_t *env)
{
  return b_binary_numerical(crisp, operands, env, operator_div);
}

static expr_t b_cons(crisp_t *crisp, expr_t operands, env_t *env)
{
  expr_t ops = crisp_eval_list(crisp, operands, env);
  EVAL_ASSERT(crisp, (length(ops) == 2), "Expected arity of 2");
  return cons(car(ops), car(cdr(ops)));
}

static expr_t b_car(crisp_t *crisp, expr_t operands, env_t *env)
{
  expr_t ops = crisp_eval_list(crisp, operands, env);
  EVAL_ASSERT(crisp, (length(ops) == 1), "Expected arity of 1");
  expr_t op = car(ops);
  EVAL_ASSERT(crisp, pair(op), "Argument must be a pair");
  return car(op);
}

static expr_t b_cdr(crisp_t *crisp, expr_t operands, env_t *env)
{
  expr_t ops = crisp_eval_list(crisp, operands, env);
  EVAL_ASSERT(crisp, (length(ops) == 1), "Expected arity of 1");
  expr_t op = car(ops);
  EVAL_ASSERT(crisp, pair(op), "Argument must be a pair");
  return cdr(op);
}

static expr_t b_length(crisp_t *crisp, expr_t operands, env_t *env)
{
  expr_t ops = crisp_eval_list(crisp, operands, env);

  EVAL_ASSERT(crisp, is_nil(cdr(ops)), "Only one argument should be passed");
  ops = car(ops);

  if(is_nil(ops)) return number_value(0.0);

  EVAL_ASSERT(crisp, pair(ops), "Operand must be a list");

  return number_value((double)length(ops));
}

static expr_t b_is_list(crisp_t *crisp, expr_t operands, env_t *env)
{
  expr_t ops = crisp_eval_list(crisp, operands, env);
  return bool_value(is_list(car(ops)));
}

void register_builtins(crisp_t *crisp)
{
  env_t *env = root_env(crisp);
  env_set(env, intern(crisp, "quote"), fn_value(&b_quote));
  env_set(env, intern(crisp, "+"), fn_value(&b_add));
  env_set(env, intern(crisp, "-"), fn_value(&b_sub));
  env_set(env, intern(crisp, "*"), fn_value(&b_mult));
  env_set(env, intern(crisp, "/"), fn_value(&b_div));
  env_set(env, intern(crisp, "cons"), fn_value(&b_cons));
  env_set(env, intern(crisp, "car"), fn_value(&b_car));
  env_set(env, intern(crisp, "cdr"), fn_value(&b_cdr));
  env_set(env, intern(crisp, "length"), fn_value(&b_length));
  env_set(env, intern(crisp, "list?"), fn_value(&b_is_list));
}
