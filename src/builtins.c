#include "builtins.h"
#include "value.h"
#include "interpreter.h"
#include "environment.h"

#define isnt intern_string_null_terminated

void register_builtins(crisp_t* crisp)
{
  env_t* env = root_env(crisp);
  env_set(env, isnt(crisp, "list?"), fn_value(&b_is_list));
}

expr_t b_is_list(crisp_t *crisp, expr_t operands)
{
  (void)crisp;
  
  expr_t c = operands;
  while(true)
  {
    if(is_cons(c))
      c = cdr(c);
    else if(is_nil(c))
      return bool_value(true);
    else
      break;
  }
  return bool_value(false);
}
