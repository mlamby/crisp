#ifndef CRISP_VALUE_H
#define CRISP_VALUE_H

#include "common.h"

typedef enum
{
  VALUE_TYPE_NIL,
  VALUE_TYPE_BOOL,
  VALUE_TYPE_NUMBER,
  VALUE_TYPE_STRING,
  VALUE_TYPE_ATOM,
  VALUE_TYPE_CONS
} value_type_t;

struct value_t
{
  value_type_t type;
  union value_store
  {
    bool boolean;
    double number;
    const char *str;
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

#define as_bool(value) ((value)->as.boolean)
#define as_number(value) ((value)->as.number)
#define as_string(value) ((value)->as.str)
#define as_atom(value) ((value)->as.str)

value_t *bool_value(bool v);
value_t *number_value(double v);
value_t *nil_value();
value_t *string_value(crisp_t* crisp, const char *chars, size_t length);
value_t *atom_value(crisp_t* crisp, const char *chars, size_t length);
value_t *atom_value_null_terminated(crisp_t* crisp, const char *chars);

value_t *cons(value_t *car, value_t *cdr);
static inline value_t *car(value_t *cons)
{
  return cons->as.cons.car;
}

static inline value_t *cdr(value_t *cons)
{
  return cons->as.cons.cdr;
}

void free_value(value_t *value);

void print_value(value_t * value);
void print_value_to_fp(value_t * value, FILE* fp);

#endif
