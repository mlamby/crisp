#include "environment.h"
#include "value.h"
#include "memory.h"

env_t *env_init()
{
  env_t *env = ALLOCATE(env_t, 1);
  env->parent = NULL;
  hash_table_init(&env->table);
  return env;
}

env_t *env_init_child(env_t *parent)
{
  env_t *env = env_init();
  env->parent = parent;
  return env;
}

bool env_is_top_level(env_t* env)
{
  return (env->parent == NULL);
}

env_t* env_get_top_level(env_t* env)
{
  while(!env_is_top_level(env))
  {
    env = env->parent;
  }

  return env;
}

void env_free(env_t *env)
{
  if(env != NULL)
  {
    env->parent = NULL;
    hash_table_free(&env->table);
    FREE(env_t, env);
  }
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

void dump_env(env_t *env)
{
  printf("Local Frame:\n");
  hash_table_dump_keys(&env->table);
}
