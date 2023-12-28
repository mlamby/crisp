#include "environment.h"
#include "value.h"
#include "memory.h"
#include "interpreter_internal.h"

static void env_free(gc_object_t* obj);

static gc_fn_t env_gc_functions = {
  .free_fn = env_free,
  .info_fn = NULL,
};

env_t *env_init(crisp_t* crisp)
{
  env_t *env = ALLOCATE(env_t, 1);
  env->parent = NULL;
  hash_table_init(&env->table);
  crisp_gc_register_object(crisp, (gc_object_t*)env, &env_gc_functions);
  return env;
}

env_t *env_init_child(crisp_t* crisp, env_t *parent)
{
  env_t *env = env_init(crisp);
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

static void env_free(gc_object_t* obj)
{
  if(obj != NULL)
  {
    env_t *env = (env_t*)obj;
    env->parent = NULL;
    hash_table_free(&env->table);
    FREE(env_t, env);
  }
}
