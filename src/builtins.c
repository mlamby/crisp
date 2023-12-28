#include "builtins.h"
#include "value.h"
#include "value_support.h"
#include "interpreter_internal.h"
#include "environment.h"
#include "evaluator.h"

#define intern intern_string_null_terminated

#define CHECK_OPERAND(c, tst, op, msg)               \
  if (!tst)                                          \
  {                                                  \
    printf("operand '");                             \
    print_value_tree(op);                                 \
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
  return number_value(crisp, result);
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
  return cons(crisp, car(ops), car(cdr(ops)));
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
  expr_t ops = eval_first_operand(crisp, operands, env);

  if (is_nil(ops))
    return number_value(crisp, 0.0);

  CHECK_OPERAND(crisp, is_proper_list(ops), ops, "must be a proper list");

  return number_value(crisp, (double)length(ops));
}

static expr_t b_is_list(crisp_t *crisp, expr_t operands, env_t *env)
{
  return bool_value(crisp, is_proper_list(eval_first_operand(crisp, operands, env)));
}

static expr_t b_not(crisp_t *crisp, expr_t operands, env_t *env)
{
  return bool_value(crisp, not(eval_first_operand(crisp, operands, env)));
}

static expr_t b_boolean(crisp_t *crisp, expr_t operands, env_t *env)
{
  return bool_value(crisp, is_bool(eval_first_operand(crisp, operands, env)));
}

static expr_t b_symbol(crisp_t *crisp, expr_t operands, env_t *env)
{
  return bool_value(crisp, is_atom(eval_first_operand(crisp, operands, env)));
}

static expr_t b_number(crisp_t *crisp, expr_t operands, env_t *env)
{
  return bool_value(crisp, is_number(eval_first_operand(crisp, operands, env)));
}

static expr_t b_string(crisp_t *crisp, expr_t operands, env_t *env)
{
  return bool_value(crisp, is_string(eval_first_operand(crisp, operands, env)));
}

static expr_t b_lambda(crisp_t *crisp, expr_t operands, env_t *env)
{
  // This is a special form. The operands are not evalutated.
  CHECK_MIN_ARITY(crisp, operands, 2U);
  return lambda_value(crisp, car(operands), cdr(operands), env);
}

static expr_t b_define(crisp_t *crisp, expr_t operands, env_t *env)
{
  CHECK_MIN_ARITY(crisp, operands, 1U);

  // Special form - the first operand is not evaluated but must be an atom.
  expr_t key = car(operands);
  CHECK_OPERAND(crisp, is_atom(key), key, "must be an atom");

  // The unassigned value in crisp is nil.
  expr_t value = nil_value(crisp);

  // If a second operand is present, evaluate it as the value for the
  // definition.
  if (length(operands) == 2)
  {
    value = crisp_eval(crisp, car(cdr(operands)), env);
  }
  else
  {
    printf("Crisp does not support abbreviated lambdas.");
    crisp_eval_error(crisp, "Unsupport form of define.");   
  }

  env_set(env_get_top_level(env), as_atom(key), value);
  return nil_value(crisp);
}

void register_builtins(crisp_t *crisp)
{
  env_t *env = root_env(crisp);
  env_set(env, intern(crisp, "quote"), fn_value(crisp, &b_quote));
  env_set(env, intern(crisp, "+"), fn_value(crisp, &b_add));
  env_set(env, intern(crisp, "-"), fn_value(crisp, &b_sub));
  env_set(env, intern(crisp, "*"), fn_value(crisp, &b_mult));
  env_set(env, intern(crisp, "/"), fn_value(crisp, &b_div));
  env_set(env, intern(crisp, "cons"), fn_value(crisp, &b_cons));
  env_set(env, intern(crisp, "list"), fn_value(crisp, &b_list));
  env_set(env, intern(crisp, "car"), fn_value(crisp, &b_car));
  env_set(env, intern(crisp, "cdr"), fn_value(crisp, &b_cdr));
  env_set(env, intern(crisp, "length"), fn_value(crisp, &b_length));
  env_set(env, intern(crisp, "list?"), fn_value(crisp, &b_is_list));
  env_set(env, intern(crisp, "not"), fn_value(crisp, &b_not));
  env_set(env, intern(crisp, "boolean?"), fn_value(crisp, &b_boolean));
  env_set(env, intern(crisp, "symbol?"), fn_value(crisp, &b_symbol));
  env_set(env, intern(crisp, "number?"), fn_value(crisp, &b_number));
  env_set(env, intern(crisp, "string?"), fn_value(crisp, &b_string));
  env_set(env, intern(crisp, "lambda"), fn_value(crisp, &b_lambda));
  env_set(env, intern(crisp, "define"), fn_value(crisp, &b_define));
}
