#include "environment.h"
#include "value.h"

void env_init(env_t *env)
{
  env->parent = NULL;
  hash_table_init(&env->table);
}

void env_init_child(env_t* env, env_t* parent)
{
  env_init(env);
  env->parent = parent;
}

void env_free(env_t *env)
{
  env->parent = NULL;
  hash_table_free(&env->table);
}

bool env_get(env_t *env, const char *name, value_t **value)
{
  bool found = hash_table_get(&env->table, name, VALUE_PTR(value));

  if ((!found) && (NULL != env->parent))
  {
    found = env_get(env->parent, name, VALUE_PTR(value));
  }

  return found;
}

void env_set(env_t *env, const char *name, value_t *value)
{
  hash_table_set(&env->table, name, value);
}

void dump_env(env_t* env)
{
  printf("Local Frame:\n");
  hash_table_dump_keys(&env->table);
}
