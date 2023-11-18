#include "simple_test.h"
#include "interpreter.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h>

int execute_crisp_code(
    const char *src,
    const char *expected,
    const char *file_name,
    int line,
    bool expect_parse_fail,
    bool run_eval,
    bool expect_eval_fail)
{
  (void)run_eval;
  crisp_t *crisp = init_interpreter();
  expr_t v = read(crisp, src);

  if (v == NULL)
  {
    if (expect_parse_fail)
    {
      free_interpreter(crisp);
      return PASS_CODE;
    }
    else
    {
      printf("\n%s(%d): Test Fail\n", file_name, line);
      printf("  : Parse fail: '%s'\n", src);
      free_interpreter(crisp);
      return FAIL_CODE;
    }
  }
  else
  {
    if (expect_parse_fail)
    {
      printf("\n%s(%d): Test Fail\n", file_name, line);
      printf("  : Expected parse to fail: '%s'\n", src);
      free_interpreter(crisp);
      return FAIL_CODE;
    }
  }

  if (run_eval)
  {
    v = eval(crisp, v, root_env(crisp));

    if (v == NULL)
    {
      if (expect_eval_fail)
      {
        free_interpreter(crisp);
        return PASS_CODE;
      }
      else
      {
        printf("\n%s(%d): Test Fail\n", file_name, line);
        printf("  : Eval fail: '%s'\n", src);
        free_interpreter(crisp);
        return FAIL_CODE;
      }
    }
    else
    {
      if (expect_eval_fail)
      {
        printf("\n%s(%d): Test Fail\n", file_name, line);
        printf("  : Expected eval to fail: '%s'\n", src);
        free_interpreter(crisp);
        return FAIL_CODE;
      }
    }
  }

  // Print the output to a file.
  FILE *fp = fopen("parse_test_output.txt", "wb");
  print_value_to_fp(v, fp);
  fclose(fp);

  // Read the file back in for comparison.
  fp = fopen("parse_test_output.txt", "rb");
  fseek(fp, 0, SEEK_END);
  size_t fsize = (size_t)ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *output = malloc(fsize + 1);
  if (fread(output, fsize, 1, fp) != 1)
  {
    printf("fread error\n");
    return FAIL_CODE;
  }

  output[fsize] = '\0';
  fclose(fp);

  int result = strcmp(output, expected);
  if (result != 0)
  {
    printf("\n%s(%d): Test Fail\n", file_name, line);
    printf("  : '%s' != '%s'\n", output, expected);
    free_interpreter(crisp);
    free(output);
    return FAIL_CODE;
  }

  free_interpreter(crisp);
  free(output);
  return PASS_CODE;
}