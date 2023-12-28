#ifndef CRISP_ENVIRONMENT_H
#define CRISP_ENVIRONMENT_H

#include "common.h"
#include "hash_table.h"
#include "gc_type.h"

struct env_t
{
  gc_object_t base;
  hash_table_t table;
  env_t* parent;
};

env_t* env_init(crisp_t* crisp);
env_t* env_init_child(crisp_t* crisp, env_t* parent);

bool env_is_top_level(env_t* env);

env_t* env_get_top_level(env_t* env);

bool env_get(env_t* env, const char* name, value_t** value);
void env_set(env_t* env, const char* name, value_t* value);

void dump_env(env_t* env);


#endif
