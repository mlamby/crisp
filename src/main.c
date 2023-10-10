#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "interpreter.h"
#include "value.h"

static void repl()
{
  char line[1024];
  crisp_t* crisp = init_interpreter();

  bool keepLooping = true;
  while (keepLooping)
  {
    printf("\n> ");

    if (fgets(line, sizeof(line), stdin))
    {
      print_value(eval(crisp, read(crisp, line), root_env(crisp)));
    }
    else
    {
      keepLooping = false;
    }
  }

  free_interpreter(crisp);
}

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  repl();
  return 0;
}
