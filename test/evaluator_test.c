#include "simple_test.h"
#include "evaluator.h"
#include "environment.h"
#include "value.h"
#include "interpreter.h"

typedef struct
{
  crisp_t *crisp;
  bool error_called;
} test_fixture_t;

static void setup(test_fixture_t *fixture);
static void teardown(test_fixture_t *fixture);
static void error_handler(crisp_t *, void *);
static bool get_value(crisp_t *crisp, const char *key, expr_t *value);

static int test_bind_env(test_fixture_t *);
static int test_bind_env_single(test_fixture_t *);
static int test_bind_env_improper_list(test_fixture_t *);
static int test_bind_env_errors(test_fixture_t *);

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  RUN_TEST_WITH_FIXTURE(test_bind_env);
  RUN_TEST_WITH_FIXTURE(test_bind_env_single);
  RUN_TEST_WITH_FIXTURE(test_bind_env_errors);
  RUN_TEST_WITH_FIXTURE(test_bind_env_improper_list);

  return PASS_CODE;
}

static int test_bind_env(test_fixture_t *f)
{
  crisp_bind_env(
      f->crisp,
      root_env(f->crisp),
      read(f->crisp, "(a b c)"),  // Formals
      read(f->crisp, "(1 2 3)")); // Parameters
  TEST_ASSERT(f->error_called == false);

  expr_t value = NULL;
  TEST_ASSERT(get_value(f->crisp, "a", &value));
  TEST_ASSERT(is_number(value));
  TEST_ASSERT(as_number(value) == 1.0);

  TEST_ASSERT(get_value(f->crisp, "b", &value));
  TEST_ASSERT(is_number(value));
  TEST_ASSERT(as_number(value) == 2.0);

  TEST_ASSERT(get_value(f->crisp, "c", &value));
  TEST_ASSERT(is_number(value));
  TEST_ASSERT(as_number(value) == 3.0);

  return PASS_CODE;
}

static int test_bind_env_single(test_fixture_t *f)
{
  crisp_bind_env(
      f->crisp,
      root_env(f->crisp),
      read(f->crisp, "a"),        // Formals
      read(f->crisp, "(1 2 3)")); // Parameters
  TEST_ASSERT(f->error_called == false);

  expr_t value = NULL;

  // Only one formal argument was provided.
  // It was a non-list so therefore all the parameters
  // should be bound to 'a' as a list
  TEST_ASSERT(get_value(f->crisp, "a", &value));
  TEST_ASSERT(is_cons(value));
  // First value in list
  TEST_ASSERT(is_number(car(value)));
  TEST_ASSERT(as_number(car(value)) == 1.0);
  // Second value in list
  TEST_ASSERT(is_number(car(cdr(value))));
  TEST_ASSERT(as_number(car(cdr(value))) == 2.0);
  // Third value in list
  TEST_ASSERT(is_number(car(cdr(cdr(value)))));
  TEST_ASSERT(as_number(car(cdr(cdr(value)))) == 3.0);
  // Only three items in the list
  TEST_ASSERT(is_nil(cdr(cdr(cdr(value)))));

  return PASS_CODE;
}

static int test_bind_env_improper_list(test_fixture_t *f)
{
  crisp_bind_env(
      f->crisp,
      root_env(f->crisp),
      read(f->crisp, "(a b . c)"),  // Formals
      read(f->crisp, "(1 2 3 4)")); // Parameters
  TEST_ASSERT(f->error_called == false);

  expr_t value = NULL;
  TEST_ASSERT(get_value(f->crisp, "a", &value));
  TEST_ASSERT(is_number(value));
  TEST_ASSERT(as_number(value) == 1.0);

  TEST_ASSERT(get_value(f->crisp, "b", &value));
  TEST_ASSERT(is_number(value));
  TEST_ASSERT(as_number(value) == 2.0);

  // Third argument 'c' should be bound to a list of
  // the remaining parameters (3 4)
  TEST_ASSERT(get_value(f->crisp, "c", &value));
  TEST_ASSERT(is_cons(value));
  // First value in list
  TEST_ASSERT(is_number(car(value)));
  TEST_ASSERT(as_number(car(value)) == 3.0);
  // Second value in list
  TEST_ASSERT(is_number(car(cdr(value))));
  TEST_ASSERT(as_number(car(cdr(value))) == 4.0);
  // Only two items in the list
  TEST_ASSERT(is_nil(cdr(cdr(value))));

  return PASS_CODE;
}

static int test_bind_env_errors(test_fixture_t *f)
{
  crisp_bind_env(
      f->crisp,
      root_env(f->crisp),
      read(f->crisp, "(1)"), // keys must be names
      read(f->crisp, "(1)"));
  TEST_ASSERT(f->error_called == true);

  f->error_called = false;
  crisp_bind_env(
      f->crisp,
      root_env(f->crisp),
      read(f->crisp, "(a b c)"), 
      read(f->crisp, "(1)"));    // Not enough parameters
  TEST_ASSERT(f->error_called == true);

  f->error_called = false;
  crisp_bind_env(
      f->crisp,
      root_env(f->crisp),
      read(f->crisp, "(a b c)"), 
      read(f->crisp, "()"));    // Not enough parameters
  TEST_ASSERT(f->error_called == true);

  f->error_called = false;
  crisp_bind_env(
      f->crisp,
      root_env(f->crisp),
      read(f->crisp, "(a b c)"), 
      read(f->crisp, "1"));    // Not enough / wrong type parameters
  TEST_ASSERT(f->error_called == true);

  return PASS_CODE;
}

static void setup(test_fixture_t *fixture)
{
  fixture->crisp = init_interpreter();
  fixture->error_called = false;
  install_error_handler(fixture->crisp, &error_handler, (void *)fixture);
}

static void teardown(test_fixture_t *fixture)
{
  free_interpreter(fixture->crisp);
}

static void error_handler(crisp_t *crisp, void *state)
{
  (void)crisp;
  ((test_fixture_t *)state)->error_called = true;
}

static bool get_value(crisp_t *crisp, const char *key, expr_t *value)
{
  return env_get(root_env(crisp), intern_string_null_terminated(crisp, key), value);
}