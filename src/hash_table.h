#ifndef CRISP_HASH_TABLE_H
#define CRISP_HASH_TABLE_H

#include "common.h"

#define VALUE_TYPE void*
#define VALUE_PTR(value) ((void*)value)

typedef struct
{
  const char* key;
  VALUE_TYPE value;
} hash_table_entry_t;

typedef struct
{
  bool is_string_table;
  size_t capacity;
  size_t size;
  hash_table_entry_t* entries;
} hash_table_t;

void hash_table_init(hash_table_t* table);
void hash_table_free(hash_table_t* table);
bool hash_table_set(hash_table_t* table, const char* key, VALUE_TYPE value);
bool hash_table_get(hash_table_t* table, const char* key, VALUE_TYPE* value);

void string_table_init(hash_table_t *table);
void string_table_free(hash_table_t* table);
const char* string_table_store(hash_table_t* table, const char* chars, size_t length);

void hash_table_dump_keys(hash_table_t* table);

#endif
