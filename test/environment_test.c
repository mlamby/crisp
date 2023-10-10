#include "simple_test.h"

#include "environment.h"
#include "value.h"

typedef struct
{
  hash_table_t strings;
  const char* v1;
  const char* v2;
  const char* v3;
} test_fixture_t;


static int env_test(test_fixture_t* fixture);
static void setup(test_fixture_t* fixture);
static void teardown(test_fixture_t* fixture);

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  RUN_TEST_WITH_FIXTURE(env_test);

  return PASS_CODE;
}

static void setup(test_fixture_t* fixture)
{
  string_table_init(&fixture->strings);
  fixture->v1 = string_table_store(&fixture->strings, "v1", 2);
  fixture->v2 = string_table_store(&fixture->strings, "v2", 2);
  fixture->v3 = string_table_store(&fixture->strings, "v3", 2);
}

static void teardown(test_fixture_t* fixture)
{
  string_table_free(&fixture->strings);
}

static int env_test(test_fixture_t* fixture)
{
  value_t *v = nil_value();
  env_t root;
  env_t child;
  env_t grand_child;
  env_init(&root);
  env_init_child(&child, &root);
  env_init_child(&grand_child, &child);

  // No values in the envs
  TEST_ASSERT(env_get(&root, fixture->v1, &v) == false);
  TEST_ASSERT(env_get(&root, fixture->v2, &v) == false);
  TEST_ASSERT(env_get(&root, fixture->v3, &v) == false);
  TEST_ASSERT(env_get(&child, fixture->v1, &v) == false);
  TEST_ASSERT(env_get(&child, fixture->v2, &v) == false);
  TEST_ASSERT(env_get(&child, fixture->v3, &v) == false);
  TEST_ASSERT(env_get(&grand_child, fixture->v1, &v) == false);
  TEST_ASSERT(env_get(&grand_child, fixture->v2, &v) == false);
  TEST_ASSERT(env_get(&grand_child, fixture->v3, &v) == false);

  // Add some values to the root environment
  env_set(&root, fixture->v1, number_value(1.0));
  env_set(&root, fixture->v2, number_value(2.0));
  env_set(&root, fixture->v3, number_value(3.0));

  // Shadow v2 and v3 in the child environment
  env_set(&child, fixture->v2, number_value(12.0));
  env_set(&child, fixture->v3, number_value(13.0));

  // Shadow v3 in the grand child environment
  env_set(&grand_child, fixture->v3, number_value(23.0));

  // All the values should be accessible through the root env.
  TEST_ASSERT(env_get(&root, fixture->v1, &v) == true);
  TEST_ASSERT(as_number(v) == 1.0);
  TEST_ASSERT(env_get(&root, fixture->v2, &v) == true);
  TEST_ASSERT(as_number(v) == 2.0);
  TEST_ASSERT(env_get(&root, fixture->v3, &v) == true);
  TEST_ASSERT(as_number(v) == 3.0);

  // All the values should be accessible through the child env.
  TEST_ASSERT(env_get(&child, fixture->v1, &v) == true);
  TEST_ASSERT(as_number(v) == 1.0);
  TEST_ASSERT(env_get(&child, fixture->v2, &v) == true);
  TEST_ASSERT(as_number(v) == 12.0);
  TEST_ASSERT(env_get(&child, fixture->v3, &v) == true);
  TEST_ASSERT(as_number(v) == 13.0);

  // All the values should be accessible through the grand child env.
  TEST_ASSERT(env_get(&grand_child, fixture->v1, &v) == true);
  TEST_ASSERT(as_number(v) == 1.0);
  TEST_ASSERT(env_get(&grand_child, fixture->v2, &v) == true);
  TEST_ASSERT(as_number(v) == 12.0);
  TEST_ASSERT(env_get(&grand_child, fixture->v3, &v) == true);
  TEST_ASSERT(as_number(v) == 23.0);

  env_free(&grand_child);
  env_free(&child);
  env_free(&root);
  return PASS_CODE;
}