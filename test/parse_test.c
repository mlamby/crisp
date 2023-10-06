#include "simple_test.h"

#include "scanner.h"
#include "parser.h"
#include "value.h"
#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>

int test_parse(const char* src, const char* expected, const char* file_name, int line, bool expect_parse_fail)
{
  crisp_t* crisp = init_interpreter();
  FILE* fp = fopen("parse_test_output.txt", "wb");
  expr_t v = parse(crisp, src);
  print_value_to_fp(v, fp);
  fclose(fp);

  if(v == NULL)
  {
    if(expect_parse_fail)
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
    if(expect_parse_fail)
    {
      printf("\n%s(%d): Test Fail\n", file_name, line);
      printf("  : Expected parse to fail: '%s'\n", src);
      free_interpreter(crisp);
      return FAIL_CODE;
    }
  }

  fp = fopen("parse_test_output.txt", "rb");
  fseek(fp, 0, SEEK_END);
  size_t fsize = (size_t)ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *output = malloc(fsize + 1);
  if(fread(output, fsize, 1, fp) != 1)
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

#define TEST_PARSE(src, exp)                                    \
  if (test_parse(src, exp, __FILE__, __LINE__, false) != PASS_CODE) {  \
    return FAIL_CODE;                                           \
  }

#define TEST_PARSE_FAILURE(src)                                       \
  if (test_parse(src, "", __FILE__, __LINE__, true) != PASS_CODE) {   \
    return FAIL_CODE;                                                 \
  }

int main(int argc, char** argv)
{
  (void)argc;
  (void)argv;

  // Braced lists
  TEST_PARSE("(     )", "()");
  TEST_PARSE("(()())", "(() ())");
  TEST_PARSE("(1)", "(1)");
  TEST_PARSE("( 1   )", "(1)");
  TEST_PARSE("(1 2 3)", "(1 2 3)");
  TEST_PARSE("(1 (2 3))", "(1 (2 3))");

  // Dotted pairs
  TEST_PARSE("(1 . (2 . (3 . ())))", "(1 2 3)");
  TEST_PARSE("(1 2 . 3)", "(1 2 . 3)");
  //  - Dot must be prior to the last datum in the list.
  TEST_PARSE_FAILURE("(1 . 2 3)");

  // Square bracket lists are treated as normal lists
  TEST_PARSE("[     ]", "()");
  TEST_PARSE("(()[])", "(() ())");
  TEST_PARSE("[()[]]", "(() ())");
  TEST_PARSE("[1 . [2 . [3 . []]]]", "(1 2 3)");
  TEST_PARSE("[1 2 . 3]", "(1 2 . 3)");
  //  - Dot must be prior to the last datum in the list.
  TEST_PARSE_FAILURE("[1 . 2 3]");

  // True false
  TEST_PARSE("(#t #f)", "(true false)");
  TEST_PARSE("(#T #F)", "(true false)");

  // Abbreviations
  TEST_PARSE("'a", "(quote a)");
  TEST_PARSE("'(1 2)", "(quote (1 2))");
  TEST_PARSE(",a", "(unquote a)");
  TEST_PARSE(",(1 2)", "(unquote (1 2))");
  TEST_PARSE("`a", "(quasiquote a)");
  TEST_PARSE("`(1 2)", "(quasiquote (1 2))");

  return PASS_CODE;
}
