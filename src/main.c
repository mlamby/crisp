#include "interpreter.h"

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  
  crisp_t* crisp = init_interpreter();

  repl(crisp);

  free_interpreter(crisp);

  return 0;
}
