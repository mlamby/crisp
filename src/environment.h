#ifndef CRISP_ENVIRONMENT_H
#define CRISP_ENVIRONMENT_H

#include "common.h"
#include "hash_table.h"

struct env_t
{
  hash_table_t table;
  env_t* parent;
};

void env_init(env_t* env);
void env_init_child(env_t* env, env_t* parent);
void env_free(env_t* env);

bool env_get(env_t* env, const char* name, value_t** value);
void env_set(env_t* env, const char* name, value_t* value);

void dump_env(env_t* env);


#endif
