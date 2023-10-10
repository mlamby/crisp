#include "interpreter.h"
#include "hash_table.h"
#include "memory.h"
#include "parser.h"
#include "value.h"
#include "symbols.h"
#include "environment.h"
#include "builtins.h"

#include <stdarg.h>

struct crisp_t
{
  hash_table_t string_table;
  symbols_t symbols;
  env_t root_env;
};

static expr_t apply(crisp_t* crisp, expr_t operator, expr_t operands);
static expr_t resolve_atom(crisp_t* crisp, expr_t node, env_t* env);

crisp_t* init_interpreter()
{
  crisp_t* crisp = ALLOCATE(crisp_t, 1);
  string_table_init(&crisp->string_table);
  reserved_symbols_init(crisp, &crisp->symbols);
  env_init(&crisp->root_env);
  register_builtins(crisp);
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

expr_t eval(crisp_t* crisp, expr_t node, env_t* env)
{
  (void)crisp;

  if(node == NULL)
    return NULL;

  if(is_bool(node) || is_string(node) || is_number(node) || is_nil(node))
    return node;

  if(is_atom(node))
  {
    return resolve_atom(crisp, node, env);  
  }

  if(is_cons(node))
  {
    if(as_bool(b_is_list(crisp, node)))
    {
      return apply(crisp, eval(crisp, car(node), env), car(cdr(node)));
    }
    else
    {
      crisp_error(crisp, "Invalid list");
      return NULL;
    }
  }
    
  return node;
}

static expr_t apply(crisp_t* crisp, expr_t operator, expr_t operands)
{
  if(is_fn(operator))
  {
    return as_fn(operator)(crisp, operands);
  }

  crisp_error(crisp, "Can not apply a non function");
  return NULL;
}

env_t* root_env(crisp_t* crisp)
{
  return &crisp->root_env;
}

const char* intern_string(crisp_t* crisp, const char* str, size_t length)
{
  const char* result = string_table_store(&crisp->string_table, str, length);
  return result;
}

const char* intern_string_null_terminated(crisp_t* crisp, const char* str)
{
  return intern_string(crisp, str, strlen(str));
}

void crisp_error(crisp_t* crisp, const char* fmt, ...)
{
  (void)crisp;
  printf("CrispError: ");
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  printf("\n");
}

static expr_t resolve_atom(crisp_t* crisp, expr_t node, env_t* env)
{
  expr_t value;
  const char* name = as_atom(node);
  if(!env_get(env, name, &value))
  {
    crisp_error(crisp, "Failed to resolve atom: <%p>%s", name, name);
    dump_env(env);
    return NULL;
  }
  return value;
}
