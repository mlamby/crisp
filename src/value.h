#ifndef CRISP_VALUE_H
#define CRISP_VALUE_H

#include "common.h"
#include "gc_type.h"

typedef enum
{
  VALUE_TYPE_NIL,
  VALUE_TYPE_BOOL,
  VALUE_TYPE_NUMBER,
  VALUE_TYPE_STRING,
  VALUE_TYPE_ATOM,
  VALUE_TYPE_CONS,
  VALUE_TYPE_FN,
  VALUE_TYPE_LAMBDA,
} value_type_t;

typedef expr_t (*fn_ptr_t)(crisp_t *, expr_t, env_t *);

typedef struct
{
  value_t *formals;
  value_t *bodies;
  env_t *env;
} lambda_t;

struct value_t
{
  gc_object_t base;
  value_type_t type;
  union value_store
  {
    bool boolean;
    double number;
    const char *str;
    fn_ptr_t fn_ptr;
    lambda_t *lambda;
    struct
    {
      value_t *car;
      value_t *cdr;
    } cons;
  } as;
};

static inline bool is_value_type(value_t const *const value, value_type_t t)
{
  return (value != NULL) && (value->type == t);
}

#define is_bool(value) (is_value_type(value, VALUE_TYPE_BOOL))
#define is_nil(value) (is_value_type(value, VALUE_TYPE_NIL))
#define is_number(value) (is_value_type(value, VALUE_TYPE_NUMBER))
#define is_string(value) (is_value_type(value, VALUE_TYPE_STRING))
#define is_atom(value) (is_value_type(value, VALUE_TYPE_ATOM))
#define is_cons(value) (is_value_type(value, VALUE_TYPE_CONS))
#define is_fn(value) (is_value_type(value, VALUE_TYPE_FN))
#define is_lambda(value) (is_value_type(value, VALUE_TYPE_LAMBDA))

#define as_bool(value) ((value)->as.boolean)
#define as_number(value) ((value)->as.number)
#define as_string(value) ((value)->as.str)
#define as_atom(value) ((value)->as.str)
#define as_fn(value) ((value)->as.fn_ptr)
#define as_lambda(value) ((value)->as.lambda)

value_t *bool_value(crisp_t *crisp, bool v);
value_t *number_value(crisp_t *crisp, double v);
value_t *nil_value(crisp_t *crisp);
value_t *string_value(crisp_t *crisp, const char *chars, size_t length);
value_t *atom_value(crisp_t *crisp, const char *chars, size_t length);
value_t *atom_value_null_terminated(crisp_t *crisp, const char *chars);
value_t *fn_value(crisp_t *crisp, fn_ptr_t ptr);
value_t *lambda_value(crisp_t* crisp, value_t* formals, value_t* bodies, env_t* env);
value_t *cons(crisp_t* crisp, value_t *car, value_t *cdr);

static inline value_t *car(value_t *cons)
{
  return cons->as.cons.car;
}

static inline value_t *cdr(value_t *cons)
{
  return cons->as.cons.cdr;
}

void print_value(value_t *value);
void print_value_to_fp(value_t *value, FILE *fp);
void print_value_tree(value_t *value);
void print_value_tree_to_fp(value_t *value, FILE *fp);

#endif
