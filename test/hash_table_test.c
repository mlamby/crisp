#include "simple_test.h"

#include "hash_table.h"

typedef struct
{
  uint32_t hash_result;
} test_fixture_t;

int hash_table_test(test_fixture_t *);
int string_table_test(test_fixture_t *);
int hash_table_tombstone_test(test_fixture_t *);

static void setup(test_fixture_t *fixture);
static void teardown(test_fixture_t *fixture);
static uint32_t fixed_hash_function(const char *key, size_t length, void *state);

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  RUN_TEST_WITH_FIXTURE(hash_table_test);
  RUN_TEST_WITH_FIXTURE(string_table_test);
  RUN_TEST_WITH_FIXTURE(hash_table_tombstone_test);

  return PASS_CODE;
}

int hash_table_test(test_fixture_t *f)
{
  (void)f;
  const char *key1 = "hello";
  const char *key2 = "yello";
  const char *key3 = "ok";

  // Empty hash table
  {
    hash_table_t table;
    hash_table_init(&table);
    TEST_ASSERT(NULL == table.entries);
    TEST_ASSERT(0 == table.capacity);
    TEST_ASSERT(0 == table.size);
    TEST_ASSERT(!table.is_string_table);
    hash_table_free(&table);
  }

  // Add with no reallocs
  {
    char *value = NULL;
    hash_table_t table;
    hash_table_init(&table);

    // Both entries don't exist yet
    TEST_ASSERT(false == hash_table_get(&table, key1, VALUE_PTR(&value)));
    TEST_ASSERT(false == hash_table_get(&table, key2, VALUE_PTR(&value)));

    // Add the first entry
    TEST_ASSERT(true == hash_table_set(&table, key1, "value_1"));
    TEST_ASSERT(NULL != table.entries);
    TEST_ASSERT(8 == table.capacity);
    TEST_ASSERT(1 == table.size);

    // First value is in the table
    TEST_ASSERT(true == hash_table_get(&table, key1, VALUE_PTR(&value)));
    TEST_ASSERT(strcmp(value, "value_1") == 0);
    // Second value isn't
    TEST_ASSERT(false == hash_table_get(&table, key2, VALUE_PTR(&value)));

    // Add the second value
    TEST_ASSERT(true == hash_table_set(&table, key2, "value_2"));
    // Now both are in the table
    TEST_ASSERT(true == hash_table_get(&table, key1, VALUE_PTR(&value)));
    TEST_ASSERT(strcmp(value, "value_1") == 0);
    TEST_ASSERT(true == hash_table_get(&table, key2, VALUE_PTR(&value)));
    TEST_ASSERT(strcmp(value, "value_2") == 0);
    TEST_ASSERT(2 == table.size);

    hash_table_free(&table);
  }

  // Test growing the capacity
  {
    char *value = NULL;
    hash_table_t table;
    hash_table_init(&table);

    TEST_ASSERT(true == hash_table_set(&table, key1, "value_1"));
    TEST_ASSERT(8 == table.capacity);
    TEST_ASSERT(true == hash_table_set(&table, key2, "value_2"));
    TEST_ASSERT(8 == table.capacity);
    TEST_ASSERT(true == hash_table_set(&table, key3, "value_3"));
    TEST_ASSERT(8 == table.capacity);
    TEST_ASSERT(true == hash_table_set(&table, "bello", "value_4"));
    TEST_ASSERT(8 == table.capacity);
    TEST_ASSERT(true == hash_table_set(&table, "qello", "value_5"));
    TEST_ASSERT(8 == table.capacity);
    TEST_ASSERT(true == hash_table_set(&table, "cello", "value_6"));
    TEST_ASSERT(8 == table.capacity);
    TEST_ASSERT(true == hash_table_set(&table, "zello", "value_7"));
    TEST_ASSERT(16 == table.capacity);
    TEST_ASSERT(7 == table.size);

    TEST_ASSERT(true == hash_table_get(&table, key1, VALUE_PTR(&value)));
    TEST_ASSERT(strcmp(value, "value_1") == 0);

    hash_table_free(&table);
  }

  return PASS_CODE;
}

int string_table_test(test_fixture_t *f)
{
  (void)f;
  // Empty string table
  {
    hash_table_t table;
    string_table_init(&table);
    TEST_ASSERT(NULL == table.entries);
    TEST_ASSERT(0 == table.capacity);
    TEST_ASSERT(0 == table.size);
    TEST_ASSERT(table.is_string_table);
    string_table_free(&table);
  }

  // Add with no reallocs
  {
    hash_table_t table;
    string_table_init(&table);

    const char *test_str = "string with key1 and key2";

    // Add the first entry
    const char *str1 = string_table_store(&table, "key1", 4);
    TEST_ASSERT(NULL != str1);
    TEST_ASSERT(NULL != table.entries);
    TEST_ASSERT(8 == table.capacity);
    TEST_ASSERT(1 == table.size);

    // First string is in the table already
    TEST_ASSERT(str1 == string_table_store(&table, "key1", 4));

    // Add the second value
    const char *str2 = string_table_store(&table, test_str + 21, 4);
    TEST_ASSERT(2 == table.size);

    // Now both are in the table
    const char *v1 = string_table_store(&table, test_str + 12, 4);
    printf("<%p>%s == <%p>%s\n", (void *)str1, str1, (void *)v1, v1);
    TEST_ASSERT(str1 == string_table_store(&table, test_str + 12, 4));
    TEST_ASSERT(str2 == string_table_store(&table, "key2", 4));

    string_table_free(&table);
  }

  return PASS_CODE;
}

int hash_table_tombstone_test(test_fixture_t *fixture)
{
  const char *key1 = "one";
  const char *key2 = "two";
  const char *key3 = "three";
  const char *key4 = "four";
  hash_table_t table;
  hash_table_init_custom_hash(&table, fixed_hash_function, fixture);

  fixture->hash_result = 0;
  TEST_ASSERT(hash_table_set(&table, key1, "k-one"));
  TEST_ASSERT(table.entries[0].key == key1);
  TEST_ASSERT(table.capacity == 8);
  TEST_ASSERT(table.size == 1);

  fixture->hash_result = 1;
  TEST_ASSERT(hash_table_set(&table, key2, "k-two"));
  TEST_ASSERT(table.entries[1].key == key2);
  TEST_ASSERT(table.size == 2);

  // Set the hash to zero so it clashes with the entry
  // already in that index.
  // The linear probe should insert it into index 2.
  fixture->hash_result = 0;
  TEST_ASSERT(hash_table_set(&table, key3, "k-three"));
  TEST_ASSERT(table.entries[2].key == key3);
  TEST_ASSERT(table.size == 3);

  // Now remove key2 which is at index 1
  fixture->hash_result = 1;
  TEST_ASSERT(hash_table_delete(&table, key2));
  TEST_ASSERT(strcmp(table.entries[1].key, "TOMBSTONE") == 0);
  TEST_ASSERT(table.size == 3); // No change - size indicates how
                                // many slots are taken.

  // Add a new entry and make sure the tombstone is skipped
  // New index should be 3.
  fixture->hash_result = 0;
  TEST_ASSERT(hash_table_set(&table, key4, "k-four"));
  TEST_ASSERT(table.entries[3].key == key4);
  TEST_ASSERT(table.size == 4);

  // Fill the rest of the table to force an increase in capacity.
  // All values are linear probed from index 0.
  fixture->hash_result = 0;
  TEST_ASSERT(table.size == 4);
  TEST_ASSERT(hash_table_set(&table, "a", "k-a"));
  TEST_ASSERT(table.size == 5);
  TEST_ASSERT(hash_table_set(&table, "b", "k-b"));
  TEST_ASSERT(table.size == 6);
  TEST_ASSERT(table.capacity == 8);

  // Next set will trigger the resize
  // Since the existing table contains a tombstone, it
  // will be remove and the size field should respect
  // the number of non-deleted items in the table -
  // 6 after the following call to set.
  TEST_ASSERT(hash_table_set(&table, "c", "k-c"));
  TEST_ASSERT(table.capacity == 16);
  TEST_ASSERT(table.size == 6);

  // Check the items are in the table and the tombstone has been
  // removed.
  TEST_ASSERT(table.entries[0].key == key1);
  TEST_ASSERT(table.entries[1].key == key3); // Key 2 has been removed.
  TEST_ASSERT(table.entries[2].key == key4);

  hash_table_free(&table);

  return PASS_CODE;
}

static void setup(test_fixture_t *fixture)
{
  fixture->hash_result = 0;
}

static void teardown(test_fixture_t *fixture)
{
  (void)fixture;
}

static uint32_t fixed_hash_function(const char *key, size_t length, void *state)
{
  (void)key;
  (void)length;
  return ((test_fixture_t *)state)->hash_result;
}
