#include "interpreter.h"
#include "hash_table.h"
#include "memory.h"
#include "parser.h"
#include "value.h"
#include "symbols.h"

typedef struct env_t env_t;
struct env_t
{
  hash_table_t table;
};

struct crisp_t
{
  hash_table_t string_table;
  symbol_state_t symbols;
  env_t root_env;
};

static void env_init(env_t* env);
//static void env_free(env_t* env);

crisp_t* init_interpreter()
{
  crisp_t* crisp = ALLOCATE(crisp_t, 1);
  string_table_init(&crisp->string_table);
  reserved_symbols_init(crisp, &crisp->symbols);
  env_init(&crisp->root_env);
  return crisp;
}

void free_interpreter(crisp_t* crisp)
{
  if(crisp != NULL)
  {
    string_table_free(&crisp->string_table);
    FREE(crisp_t, crisp);
  }
}

expr_t read(crisp_t* crisp, const char* source)
{
  return parse(crisp, source);
}

expr_t eval(crisp_t* crisp, expr_t node)
{
  (void)crisp;

  if(node == NULL)
    return nil_value();

  if(is_bool(node) || is_string(node) || is_number(node) || is_nil(node))
    return node;

  if(is_atom(node))
    return nil_value();
    
  return node;
}

const char* intern_string(crisp_t* crisp, const char* str, size_t length)
{
  return string_table_store(&crisp->string_table, str, length);
}

const char* intern_string_null_terminated(crisp_t* crisp, const char* str)
{
  return intern_string(crisp, str, strlen(str));
}

env_t* root_environment(crisp_t* env)
{
  (void)env;
  return NULL;
}

static void env_init(env_t* env)
{
  string_table_init(&env->table);
}

// static void env_free(env_t* env)
// {
//   (void)env;
// }
