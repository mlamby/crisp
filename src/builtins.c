#include "builtins.h"
#include "value.h"
#include "value_support.h"
#include "interpreter.h"
#include "environment.h"
#include "evaluator.h"

#define intern intern_string_null_terminated

#define CHECK_OPERAND(c, tst, op, msg)               \
  if (!tst)                                          \
  {                                                  \
    printf("operand '");                             \
    print_value(op);                                 \
    printf("' failed check: %s\n", msg);             \
    crisp_eval_error(crisp, "Operand check failed"); \
    return NULL;                                     \
  }

#define CHECK_ARITY(c, ops, sz)                         \
  size_t len = length(ops);                             \
  if (len != sz)                                        \
  {                                                     \
    printf("Expected arity of %u, got %zu\n", sz, len); \
    crisp_eval_error(crisp, "Arity");                   \
  }

#define CHECK_MIN_ARITY(c, ops, sz)                             \
  size_t len = length(ops);                                     \
  if (sz > len)                                                 \
  {                                                             \
    printf("Expected minimum arity of %u, got %zu\n", sz, len); \
    crisp_eval_error(crisp, "Min Arity");                       \
  }

static expr_t eval_first_operand(crisp_t *crisp, expr_t operands, env_t *env)
{
  CHECK_ARITY(crisp, operands, 1U);
  return crisp_eval(crisp, car(operands), env);
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

  while ((operand = iter_next(&iter)) != NULL)
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
  CHECK_ARITY(crisp, ops, 2U);
  return cons(car(ops), car(cdr(ops)));
}

static expr_t b_list(crisp_t *crisp, expr_t operands, env_t *env)
{
  return crisp_eval_list(crisp, operands, env);
}

static expr_t b_car(crisp_t *crisp, expr_t operands, env_t *env)
{
  expr_t op = eval_first_operand(crisp, operands, env);
  CHECK_OPERAND(crisp, pair(op), op, "must be a pair");
  return car(op);
}

static expr_t b_cdr(crisp_t *crisp, expr_t operands, env_t *env)
{
  expr_t op = eval_first_operand(crisp, operands, env);
  CHECK_OPERAND(crisp, pair(op), op, "must be a pair");
  return cdr(op);
}

static expr_t b_length(crisp_t *crisp, expr_t operands, env_t *env)
{
  expr_t ops = crisp_eval_list(crisp, operands, env);
  CHECK_ARITY(crisp, ops, 1U);
  ops = car(ops);

  if (is_nil(ops))
    return number_value(0.0);
  CHECK_OPERAND(crisp, pair(ops), ops, "must be a list");

  return number_value((double)length(ops));
}

static expr_t b_is_list(crisp_t *crisp, expr_t operands, env_t *env)
{
  return bool_value(is_proper_list(eval_first_operand(crisp, operands, env)));
}

static expr_t b_not(crisp_t *crisp, expr_t operands, env_t *env)
{
  return bool_value(not(eval_first_operand(crisp, operands, env)));
}

static expr_t b_boolean(crisp_t *crisp, expr_t operands, env_t *env)
{
  return bool_value(is_bool(eval_first_operand(crisp, operands, env)));
}

static expr_t b_symbol(crisp_t *crisp, expr_t operands, env_t *env)
{
  return bool_value(is_atom(eval_first_operand(crisp, operands, env)));
}

static expr_t b_number(crisp_t *crisp, expr_t operands, env_t *env)
{
  return bool_value(is_number(eval_first_operand(crisp, operands, env)));
}

static expr_t b_string(crisp_t *crisp, expr_t operands, env_t *env)
{
  return bool_value(is_string(eval_first_operand(crisp, operands, env)));
}

static expr_t b_lambda(crisp_t *crisp, expr_t operands, env_t *env)
{
  // This is a special form. The operands are not evalutated.
  CHECK_MIN_ARITY(crisp, operands, 2U);
  return lambda_value(crisp, car(operands), cdr(operands), env);
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
  env_set(env, intern(crisp, "list"), fn_value(&b_list));
  env_set(env, intern(crisp, "car"), fn_value(&b_car));
  env_set(env, intern(crisp, "cdr"), fn_value(&b_cdr));
  env_set(env, intern(crisp, "length"), fn_value(&b_length));
  env_set(env, intern(crisp, "list?"), fn_value(&b_is_list));
  env_set(env, intern(crisp, "not"), fn_value(&b_not));
  env_set(env, intern(crisp, "boolean?"), fn_value(&b_boolean));
  env_set(env, intern(crisp, "symbol?"), fn_value(&b_symbol));
  env_set(env, intern(crisp, "number?"), fn_value(&b_number));
  env_set(env, intern(crisp, "string?"), fn_value(&b_string));
  env_set(env, intern(crisp, "lambda"), fn_value(&b_lambda));
}
