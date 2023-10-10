#include "simple_test.h"

#include "value.h"
#include "interpreter.h"

static expr_t captured = NULL;
expr_t sample_fn(crisp_t* a1, expr_t a2)
{
  (void)a1;
  captured = a2;
  return number_value(123);
}

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  crisp_t *crisp = init_interpreter();

  {
    value_t *v = bool_value(true);
    TEST_ASSERT(v->type == VALUE_TYPE_BOOL);
    TEST_ASSERT(v->as.boolean == true);
    TEST_ASSERT(is_bool(v) == true);
    TEST_ASSERT(is_nil(v) == false);
    TEST_ASSERT(is_number(v) == false);
    TEST_ASSERT(is_string(v) == false);
    TEST_ASSERT(is_atom(v) == false);
    TEST_ASSERT(is_cons(v) == false);
    TEST_ASSERT(is_fn(v) == false);
    TEST_ASSERT(as_bool(v) == true);
    free_value(v);
  }

  {
    value_t *v = bool_value(false);
    TEST_ASSERT(v->type == VALUE_TYPE_BOOL);
    TEST_ASSERT(v->as.boolean == false);
    TEST_ASSERT(is_bool(v) == true);
    TEST_ASSERT(is_nil(v) == false);
    TEST_ASSERT(is_number(v) == false);
    TEST_ASSERT(is_string(v) == false);
    TEST_ASSERT(is_atom(v) == false);
    TEST_ASSERT(is_cons(v) == false);
    TEST_ASSERT(is_fn(v) == false);
    TEST_ASSERT(as_bool(v) == false);
    free_value(v);
  }

  {
    value_t *v = nil_value();
    TEST_ASSERT(v->type == VALUE_TYPE_NIL);
    TEST_ASSERT(is_bool(v) == false);
    TEST_ASSERT(is_nil(v) == true);
    TEST_ASSERT(is_number(v) == false);
    TEST_ASSERT(is_string(v) == false);
    TEST_ASSERT(is_atom(v) == false);
    TEST_ASSERT(is_cons(v) == false);
    TEST_ASSERT(is_fn(v) == false);
    free_value(v);
  }

  {
    value_t *v = number_value(1.0);
    TEST_ASSERT(v->type == VALUE_TYPE_NUMBER);
    TEST_ASSERT(v->as.number == 1.0);
    TEST_ASSERT(is_bool(v) == false);
    TEST_ASSERT(is_nil(v) == false);
    TEST_ASSERT(is_number(v) == true);
    TEST_ASSERT(is_string(v) == false);
    TEST_ASSERT(is_atom(v) == false);
    TEST_ASSERT(is_cons(v) == false);
    TEST_ASSERT(is_fn(v) == false);
    TEST_ASSERT(as_number(v) == 1.0);
    free_value(v);
  }

  {
    value_t *v = string_value(crisp, "hello", 5);
    TEST_ASSERT(v->type == VALUE_TYPE_STRING);
    TEST_ASSERT(is_bool(v) == false);
    TEST_ASSERT(is_nil(v) == false);
    TEST_ASSERT(is_number(v) == false);
    TEST_ASSERT(is_string(v) == true);
    TEST_ASSERT(is_atom(v) == false);
    TEST_ASSERT(is_cons(v) == false);
    TEST_ASSERT(is_fn(v) == false);
    TEST_ASSERT(strcmp("hello", as_string(v)) == 0);
    TEST_ASSERT(strlen(as_string(v)) == 5);
    free_value(v);
  }

  {
    value_t *v = atom_value(crisp, ":x", 2);
    TEST_ASSERT(v->type == VALUE_TYPE_ATOM);
    TEST_ASSERT(is_bool(v) == false);
    TEST_ASSERT(is_nil(v) == false);
    TEST_ASSERT(is_number(v) == false);
    TEST_ASSERT(is_string(v) == false);
    TEST_ASSERT(is_atom(v) == true);
    TEST_ASSERT(is_cons(v) == false);
    TEST_ASSERT(is_fn(v) == false);
    TEST_ASSERT(strcmp(":x", as_atom(v)) == 0);
    TEST_ASSERT(strlen(as_atom(v)) == 2);
    free_value(v);
  }

  {
    value_t *v = cons(bool_value(true), nil_value());
    TEST_ASSERT(v->type == VALUE_TYPE_CONS);
    TEST_ASSERT(is_bool(v) == false);
    TEST_ASSERT(is_nil(v) == false);
    TEST_ASSERT(is_number(v) == false);
    TEST_ASSERT(is_string(v) == false);
    TEST_ASSERT(is_atom(v) == false);
    TEST_ASSERT(is_cons(v) == true);
    TEST_ASSERT(is_fn(v) == false);

    TEST_ASSERT(is_bool(car(v)) == true);
    TEST_ASSERT(is_nil(cdr(v)) == true);
    free_value(v);
  }

  {
    captured = NULL;
    value_t *v = fn_value(&sample_fn);
    TEST_ASSERT(v->type == VALUE_TYPE_FN);
    TEST_ASSERT(is_bool(v) == false);
    TEST_ASSERT(is_nil(v) == false);
    TEST_ASSERT(is_number(v) == false);
    TEST_ASSERT(is_string(v) == false);
    TEST_ASSERT(is_atom(v) == false);
    TEST_ASSERT(is_cons(v) == false);
    TEST_ASSERT(is_fn(v) == true);

    value_t* r = as_fn(v)(NULL, number_value(1));
    TEST_ASSERT(as_number(r) == 123.0);
    TEST_ASSERT(as_number(captured) == 1.0);

    free_value(v);
  }

  free_interpreter(crisp);
}
