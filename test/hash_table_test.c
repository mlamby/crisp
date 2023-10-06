#include "simple_test.h"

#include "hash_table.h"

int hash_table_test();
int string_table_test();

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  RUN_TEST(hash_table_test);
  RUN_TEST(string_table_test);

  return PASS_CODE;
}

int hash_table_test()
{
  const char* key1 = "hello";
  const char* key2 = "yello";
  const char* key3 = "ok";

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
    char* value = NULL;
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
    char* value = NULL;
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

int string_table_test()
{
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

    // Add the first entry
    const char* str1 = string_table_store(&table, "key1", 4);
    TEST_ASSERT(NULL != str1);
    TEST_ASSERT(NULL != table.entries);
    TEST_ASSERT(8 == table.capacity);
    TEST_ASSERT(1 == table.size);

    // First string is in the table already
    TEST_ASSERT(str1 == string_table_store(&table, "key1", 4));

    // Add the second value
    const char* str2 = string_table_store(&table, "key2", 4);
    TEST_ASSERT(2 == table.size);

    // Now both are in the table
    TEST_ASSERT(str1 == string_table_store(&table, "key1", 4));
    TEST_ASSERT(str2 == string_table_store(&table, "key2", 4));

    string_table_free(&table);
  }

  return PASS_CODE;
}
