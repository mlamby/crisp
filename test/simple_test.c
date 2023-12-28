#include "simple_test.h"
#include "interpreter_internal.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h>

int execute_crisp_code(
    crisp_t* crisp,
    const char *src,
    const char *expected,
    const char *file_name,
    int line,
    bool expect_parse_fail,
    bool run_eval,
    bool expect_eval_fail)
{
  (void)run_eval;
  expr_t v = read(crisp, src);

  if (v == NULL)
  {
    if (expect_parse_fail)
    {
      return PASS_CODE;
    }
    else
    {
      printf("\n%s(%d): Test Fail\n", file_name, line);
      printf("  : Parse fail: '%s'\n", src);
      return FAIL_CODE;
    }
  }
  else
  {
    if (expect_parse_fail)
    {
      printf("\n%s(%d): Test Fail\n", file_name, line);
      printf("  : Expected parse to fail: '%s'\n", src);
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
        return PASS_CODE;
      }
      else
      {
        printf("\n%s(%d): Test Fail\n", file_name, line);
        printf("  : Eval fail: '%s'\n", src);
        return FAIL_CODE;
      }
    }
    else
    {
      if (expect_eval_fail)
      {
        printf("\n%s(%d): Test Fail\n", file_name, line);
        printf("  : Expected eval to fail: '%s'\n", src);
        return FAIL_CODE;
      }
    }
  }

  int result = compare_crisp_value(v, expected, file_name, line);
  return result;
}

int compare_crisp_value(
  expr_t value,
  const char* expected,
  const char* file_name, 
  int line)
{
  int result = PASS_CODE;

  // Print the output to a file.
  FILE *fp = fopen("parse_test_output.txt", "wb");
  print_value_tree_to_fp(value, fp);
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
    free(output);
    fclose(fp);
    return FAIL_CODE;
  }

  output[fsize] = '\0';
  fclose(fp);

  if (strcmp(output, expected) != 0)
  {
    printf("\n%s(%d): Test Fail\n", file_name, line);
    printf("  : '%s' != '%s'\n", output, expected);
    result = FAIL_CODE;
  }

  free(output);

  return result;
}
