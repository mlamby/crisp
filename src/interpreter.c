#include "interpreter.h"
#include "hash_table.h"
#include "memory.h"
#include "parser.h"
#include "environment.h"
#include "evaluator.h"
#include "builtins.h"
#include "value.h"

#include <stdarg.h>
#include <setjmp.h>

static jmp_buf sJumpBuffer;

struct crisp_t
{
  hash_table_t string_table;
  env_t root_env;
};

crisp_t* init_interpreter()
{
  crisp_t* crisp = ALLOCATE(crisp_t, 1);
  string_table_init(&crisp->string_table);
  env_init(&crisp->root_env);
  register_builtins(crisp);
  return crisp;
}

void free_interpreter(crisp_t* crisp)
{
  if(crisp != NULL)
  {
    string_table_free(&crisp->string_table);
    env_free(&crisp->root_env);
    FREE(crisp_t, crisp);
  }
}

expr_t read(crisp_t* crisp, const char* source)
{
  return parse(crisp, source);
}

expr_t eval(crisp_t* crisp, expr_t node, env_t* env)
{
  expr_t result = NULL;
  if(setjmp(sJumpBuffer) == CRISP_ERROR_NONE)
  {
    result = crisp_eval(crisp, node, env);
  }
  return result;
}

void repl(crisp_t* crisp)
{
  char line[1024];

  // Run the REPL loop
  while (true)
  {
    printf("\n> ");

    if (fgets(line, sizeof(line), stdin))
    {
      print_value(eval(crisp, read(crisp, line), root_env(crisp)));
    }
    else
    {
      break;
    }
  }
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

void crisp_error_jump(crisp_t* crisp, crisp_error_t err)
{
  (void)crisp;
  if(err != CRISP_ERROR_NONE)
  {
    longjmp(sJumpBuffer, (int)err);
  }
}
