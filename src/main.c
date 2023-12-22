#include "interpreter.h"
#include "memory.h"

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  memory_install_logging("crisp.mem.txt");
  
  crisp_t* crisp = init_interpreter();

  repl(crisp);

  free_interpreter(crisp);

  return 0;
}
