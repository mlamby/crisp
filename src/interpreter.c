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
#include <signal.h>

static jmp_buf sJumpBuffer;
static sig_atomic_t sSignal = 0;
static bool sHandlerInstalled = false;

void signal_handler(int signal)
{
  sSignal = signal;
}

struct crisp_t
{
  hash_table_t string_table;
  env_t* root_env;
  error_handler_t handler_fn;
  void *handler_state;
  bool jump_buffer_ready;
};

crisp_t *init_interpreter()
{
  crisp_t *crisp = ALLOCATE(crisp_t, 1);
  string_table_init(&crisp->string_table);
  crisp->root_env = env_init();
  crisp->jump_buffer_ready = false;
  crisp->handler_fn = NULL;
  crisp->handler_state = NULL;
  register_builtins(crisp);

  if(!sHandlerInstalled)
  {
    signal(SIGINT, signal_handler);
    sHandlerInstalled = true;
  }
  return crisp;
}

void free_interpreter(crisp_t *crisp)
{
  if (crisp != NULL)
  {
    string_table_free(&crisp->string_table);
    env_free(crisp->root_env);
    FREE(crisp_t, crisp);
  }
}

void install_error_handler(crisp_t *crisp, error_handler_t handler, void *handler_state)
{
  crisp->handler_fn = handler;
  crisp->handler_state = handler_state;
}

expr_t read(crisp_t *crisp, const char *source)
{
  return parse(crisp, source);
}

expr_t eval(crisp_t *crisp, expr_t node, env_t *env)
{
  expr_t result = NULL;
  crisp->jump_buffer_ready = true;

  if (setjmp(sJumpBuffer) == CRISP_ERROR_NONE)
  {
    result = crisp_eval(crisp, node, env);
  }

  crisp->jump_buffer_ready = false;
  return result;
}

void repl(crisp_t *crisp)
{
  char line[1024];

  // Run the REPL loop
  while (sSignal == 0)
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

env_t *root_env(crisp_t *crisp)
{
  return crisp->root_env;
}

const char *intern_string(crisp_t *crisp, const char *str, size_t length)
{
  const char *result = string_table_store(&crisp->string_table, str, length);
  return result;
}

const char *intern_string_null_terminated(crisp_t *crisp, const char *str)
{
  return intern_string(crisp, str, strlen(str));
}

void crisp_error_jump(crisp_t *crisp, crisp_error_t err)
{
  (void)crisp;
  if (err != CRISP_ERROR_NONE)
  {
    if (crisp->handler_fn)
    {
      crisp->handler_fn(crisp, crisp->handler_state);
    }

    if (crisp->jump_buffer_ready)
    {
      longjmp(sJumpBuffer, (int)err);
    }
  }
}
