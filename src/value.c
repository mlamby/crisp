#include "value.h"
#include "memory.h"
#include "interpreter_internal.h"

#include <stdlib.h>
#include <stdio.h>

// TODO: Optimisation - have nil_value and bool_value return the same
// instance every time.

static value_t *allocate_value(crisp_t *crisp, value_type_t type);
static void free_value(gc_object_t *value);
static void print_gc_value(gc_object_t *value);

static gc_fn_t value_gc_functions = {
  .free_fn = free_value,
  .info_fn = print_gc_value,
};

value_t *bool_value(crisp_t *crisp, bool v)
{
  value_t *value = allocate_value(crisp, VALUE_TYPE_BOOL);
  value->as.boolean = v;
  return value;
}

value_t *number_value(crisp_t *crisp, double v)
{
  value_t *value = allocate_value(crisp, VALUE_TYPE_NUMBER);
  value->as.number = v;
  return value;
}

value_t *nil_value(crisp_t *crisp)
{
  return allocate_value(crisp, VALUE_TYPE_NIL);
}

value_t *string_value(crisp_t *crisp, const char *chars, size_t length)
{
  value_t *value = allocate_value(crisp, VALUE_TYPE_STRING);
  value->as.str = intern_string(crisp, chars, length);
  return value;
}

value_t *atom_value(crisp_t *crisp, const char *chars, size_t length)
{
  value_t *value = allocate_value(crisp, VALUE_TYPE_ATOM);
  value->as.str = intern_string(crisp, chars, length);
  return value;
}

value_t *atom_value_null_terminated(crisp_t *crisp, const char *chars)
{
  return atom_value(crisp, chars, strlen(chars));
}

value_t *fn_value(crisp_t *crisp, fn_ptr_t ptr)
{
  value_t *value = allocate_value(crisp, VALUE_TYPE_FN);
  value->as.fn_ptr = ptr;
  return value;
}

value_t *lambda_value(crisp_t *crisp, value_t *formals, value_t *bodies, env_t *env)
{
  (void)crisp;

  value_t *value = allocate_value(crisp, VALUE_TYPE_LAMBDA);
  lambda_t *lambda = ALLOCATE(lambda_t, 1);
  value->as.lambda = lambda;

  lambda->formals = formals;
  lambda->bodies = bodies;
  lambda->env = env;

  return value;
}

value_t *cons(crisp_t* crisp, value_t *car, value_t *cdr)
{
  value_t *value = allocate_value(crisp, VALUE_TYPE_CONS);
  value->as.cons.car = car;
  value->as.cons.cdr = cdr;
  return value;
}

void print_value(value_t *value)
{
  print_value_to_fp(value, stdout);
}

void print_value_to_fp(value_t *value, FILE *fp)
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
  else if (is_number(value))
  {
    fprintf(fp, "%g", as_number(value));
  }
  else if (is_string(value))
  {
    fprintf(fp, "\"%s\"", as_string(value));
  }
  else if (is_atom(value))
  {
    fprintf(fp, "%s", as_atom(value));
  }
  else if (is_fn(value))
  {
    fprintf(fp, "<builtin:%p>", (void*)value->as.fn_ptr);
  }
  else if (is_lambda(value))
  {
    fprintf(fp, "<lambda:%p>", (void*)value->as.lambda);
  }
  else if (is_cons(value))
  {
    fprintf(fp, "<cons>");
  }
}

void print_value_tree(value_t *value)
{
  print_value_tree_to_fp(value, stdout);
}

void print_value_tree_to_fp(value_t *value, FILE *fp)
{
  if (value == NULL)
  {
    return;
  }

  if (is_cons(value))
  {
    fprintf(fp, "(");
    while (true)
    {
      print_value_tree_to_fp(car(value), fp);
      value = cdr(value);
      if (is_nil(value))
        break;
      if (is_cons(value))
      {
        fprintf(fp, " ");
      }
      else
      {
        fprintf(fp, " . ");
        print_value_tree_to_fp(value, fp);
        break;
      }
    }
    fprintf(fp, ")");
  }
  else
  {
    print_value_to_fp(value, fp);
  }
}

static value_t *allocate_value(crisp_t *crisp, value_type_t type)
{
  value_t *value = ALLOCATE(value_t, 1);
  value->type = type;
  crisp_gc_register_object(crisp, (gc_object_t*)value, &value_gc_functions);
  return value;
}

static void free_value(gc_object_t *obj)
{
  value_t *value = (value_t *)obj;
  if (is_cons(value))
  {
    value->as.cons.car = NULL;
    value->as.cons.cdr = NULL;
    // Don't free the cons and car values.
    // The GC will pick them up.
  }
  else if (is_lambda(value))
  {
    FREE(lambda_t, value->as.lambda);
    value->as.lambda = NULL;
  }
  FREE(value_t, value);
}

static void print_gc_value(gc_object_t *value)
{
  print_value((value_t *)value);
}
