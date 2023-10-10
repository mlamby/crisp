#include "value.h"
#include "memory.h"
#include "interpreter.h"

#include <stdlib.h>
#include <stdio.h>

// TODO: Optimisation - have nil_value and bool_value return the same
// instance every time.

static value_t *allocate_value(value_type_t type);

value_t *bool_value(bool v)
{
  value_t *value = allocate_value(VALUE_TYPE_BOOL);
  value->as.boolean = v;
  return value;
}

value_t *number_value(double v)
{
  value_t *value = allocate_value(VALUE_TYPE_NUMBER);
  value->as.number = v;
  return value;
}

value_t *nil_value()
{
  return allocate_value(VALUE_TYPE_NIL);
}

value_t *string_value(crisp_t* crisp, const char *chars, size_t length)
{
  value_t *value = allocate_value(VALUE_TYPE_STRING);
  value->as.str = intern_string(crisp, chars, length);
  return value;
}

value_t *atom_value(crisp_t* crisp, const char *chars, size_t length)
{
  value_t *value = allocate_value(VALUE_TYPE_ATOM);
  value->as.str = intern_string(crisp, chars, length);
  return value;
}

value_t *atom_value_null_terminated(crisp_t* crisp, const char *chars)
{
  return atom_value(crisp, chars, strlen(chars));
}

value_t *fn_value(fn_ptr_t ptr)
{
  value_t *value = allocate_value(VALUE_TYPE_FN);
  value->as.fn_ptr = ptr;
  return value;
}

value_t *cons(value_t *car, value_t *cdr)
{
  value_t *value = allocate_value(VALUE_TYPE_CONS);
  value->as.cons.car = car;
  value->as.cons.cdr = cdr;
  return value;
}

void free_value(value_t *value)
{
  if (is_cons(value))
  {
    free_value(value->as.cons.car);
    value->as.cons.car = NULL;
    free_value(value->as.cons.cdr);
    value->as.cons.cdr = NULL;
  }
  reallocate(value, 0, 0);
}

void print_value(value_t *value)
{
  print_value_to_fp(value, stdout);
}

void print_value_to_fp(value_t *value, FILE* fp)
{
  if (value == NULL)
  {
    return;
  }

  if (is_nil(value))
  {
    fprintf(fp, "()");
  }
  else if (is_bool(value))
  {
    if (as_bool(value) == true)
      fprintf(fp, "true");
    else
      fprintf(fp, "false");
  }
  else if(is_number(value))
  {
    fprintf(fp, "%g", as_number(value));
  }
  else if(is_string(value))
  {
    fprintf(fp, "\"%s\"", as_string(value));
  }
  else if(is_atom(value))
  {
    fprintf(fp, "%s", as_atom(value));
  }
  else if(is_fn(value))
  {
    fprintf(fp, "<builtin>");
  }
  else if(is_cons(value))
  {
    fprintf(fp, "(");
    while(true)
    {
      print_value_to_fp(car(value), fp);
      value = cdr(value);
      if(is_nil(value)) break;
      if(is_cons(value))
      {
        fprintf(fp, " ");
      }
      else
      {
        fprintf(fp, " . ");
        print_value_to_fp(value, fp);
        break;
      }
    }
    fprintf(fp, ")");
  }
}

static value_t *allocate_value(value_type_t type)
{
  value_t *value = (value_t *)reallocate(NULL, 0, sizeof(value_t));
  value->type = type;
  return value;
}
