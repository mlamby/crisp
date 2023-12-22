#include "hash_table.h"
#include "memory.h"

#include <string.h>
#include <stdio.h>

static const float sLoadFactor = 0.75;
static const size_t sMinCapacity = 8;
static const size_t sCapacityMultiplier = 2;
static const char* TOMBSTONE = "TOMBSTONE";

static uint32_t hash_string(const char *key, size_t length, void* state);
static void increase_capacity(hash_table_t *table, size_t new_capacity);
static void increase_capacity_if_required(hash_table_t *table);
static hash_table_entry_t *find_entry(
  hash_table_t* table,
  hash_table_entry_t *entries,
  size_t capacity,
  const char *key,
  bool compare_string_contents,
  size_t key_len);
static bool key_match(const char *key1, const char *key2, bool compare_string_contents, size_t key_len);

void hash_table_init(hash_table_t *table)
{
  hash_table_init_custom_hash(table, hash_string, NULL);
}

void hash_table_init_custom_hash(hash_table_t* table, hash_fn_t hash_fn, void* hash_state)
{
  table->is_string_table = false;
  table->capacity = 0;
  table->size = 0;
  table->entries = NULL;
  table->hash_fn = hash_fn;
  table->hash_fn_state = hash_state;
}

void hash_table_free(hash_table_t *table)
{
  if (table->capacity > 0)
  {
    FREE_ARRAY(hash_table_entry_t, table->entries, table->capacity);
    table->size = 0;
    table->capacity = 0;
  }
}

bool hash_table_set(hash_table_t *table, const char *key, VALUE_TYPE value)
{
  increase_capacity_if_required(table);

  hash_table_entry_t *e = find_entry(
      table,
      table->entries,
      table->capacity,
      key,
      false,
      0);

  bool new_key = false;
  if (e != NULL)
  {
    new_key = (e->key == NULL);

    if (new_key)
    {
      e->key = key;
      ++table->size;
    }

    e->value = value;
  }

  return new_key;
}

bool hash_table_get(hash_table_t *table, const char *key, VALUE_TYPE *value)
{
  hash_table_entry_t *e = find_entry(
      table,
      table->entries,
      table->capacity,
      key,
      false,
      0);

  bool found_key = (e != NULL) && (e->key != NULL);
  if (found_key)
  {
    *value = e->value;
  }
  return found_key;
}

bool hash_table_delete(hash_table_t* table, const char* key)
{
  hash_table_entry_t *e = find_entry(
      table,
      table->entries,
      table->capacity,
      key,
      false,
      0);

  bool found_key = (e != NULL) && (e->key != NULL);
  if (found_key)
  {
    e->key = TOMBSTONE;
    e->value = NULL;
  }
  return found_key;
}

void string_table_init(hash_table_t *table)
{
  hash_table_init(table);
  table->is_string_table = true;
}

void string_table_free(hash_table_t *table)
{
  for (size_t i = 0; i < table->capacity; ++i)
  {
    if(table->entries[i].key != NULL)
    {
      FREE_ARRAY(char, (void *)(table->entries[i].key), strlen(table->entries[i].key) + 1);
    }
  }

  hash_table_free(table);
}

const char *string_table_store(hash_table_t *table, const char *chars, size_t length)
{
  increase_capacity_if_required(table);

  hash_table_entry_t *e = find_entry(
      table,
      table->entries,
      table->capacity,
      chars,
      true,
      length);

  const char *result = NULL;

  if (e != NULL)
  {
    if (e->key == NULL)
    {
      char *heapChars = ALLOCATE(char, length + 1);
      memcpy(heapChars, chars, length);
      heapChars[length] = '\0';
      e->key = heapChars;
      ++table->size;
    }

    e->value = NULL;
    result = e->key;
  }

  return result;
}

void hash_table_dump_keys(hash_table_t *table)
{
  for (size_t i = 0; i < table->capacity; ++i)
  {
    hash_table_entry_t *e = &(table->entries[i]);
    if (NULL != e->key)
    {
      printf("[idx:%zu] <%p>'%s'\n", i, (void*)e->key, e->key);
    }
  }
}

static uint32_t hash_string(const char *key, size_t length, void* state)
{
  (void)state;
  // FNV-1a algorithm taken directly from
  // https://craftinginterpreters.com/hash-tables.html
  uint32_t hash = 2166136261u;
  for (size_t i = 0; i < length; i++)
  {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

static void increase_capacity(hash_table_t *table, size_t new_capacity)
{
  size_t new_size = 0;
  hash_table_entry_t *new_list = ALLOCATE(hash_table_entry_t, new_capacity);
  memset(new_list, 0, sizeof(hash_table_entry_t) * new_capacity);

  // Copy all the entries from the existing table into a new slot
  // in the new table.
  for (size_t i = 0; i < table->capacity; ++i)
  {
    hash_table_entry_t *old_entry = &(table->entries[i]);
    if ((old_entry->key != NULL) && (old_entry->key != TOMBSTONE))
    {
      // Find a spot in the new table for the entry
      hash_table_entry_t *new_slot = find_entry(
          table,
          new_list,
          new_capacity,
          old_entry->key,
          table->is_string_table,
          0);

      new_slot->key = old_entry->key;
      new_slot->value = old_entry->value;
      new_size++;
    }
  }

  // Clear the old list and replace it with the new one
  FREE_ARRAY(hash_table_entry_t, table->entries, table->capacity);
  table->entries = new_list;
  table->capacity = new_capacity;
  table->size = new_size;
}

static void increase_capacity_if_required(hash_table_t *table)
{
  if ((table->size + 1) > (size_t)((float)table->capacity * sLoadFactor))
  {
    size_t new_capacity = table->capacity * sCapacityMultiplier;
    if (new_capacity < sMinCapacity)
    {
      new_capacity = sMinCapacity;
    }
    increase_capacity(table, new_capacity);
  }
}

static hash_table_entry_t *find_entry(
    hash_table_t* table,
    hash_table_entry_t *entries,
    size_t capacity,
    const char *key,
    bool compare_string_contents,
    size_t key_len)
{
  if ((entries == NULL) || (capacity == 0))
  {
    return NULL;
  }

  if(key_len == 0)
  {
    key_len = strlen(key);
  }
  uint32_t hash = table->hash_fn(key, key_len, table->hash_fn_state);
  size_t index = hash % capacity;

  bool searching = true;
  hash_table_entry_t *found_entry = NULL;

  while (searching)
  {
    hash_table_entry_t *e = &(entries[index]);
    if (e->key != NULL)
    {
      if (key_match(e->key, key, compare_string_contents, key_len))
      {
        // The key is already in the table.
        found_entry = e;
        searching = false;
      }
      else
      {
        // Linear probe
        index = (index + 1) % capacity;
      }
    }
    else
    {
      // Found an empty slot so the key must not already be in the table
      found_entry = e;
      searching = false;
    }
  }

  return found_entry;
}

static bool key_match(const char *key1, const char *key2, bool compare_string_contents, size_t key_len)
{
  if (compare_string_contents)
  {
    return (strncmp(key1, key2, key_len) == 0);
  }
  return key1 == key2;
}
