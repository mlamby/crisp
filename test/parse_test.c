#include "simple_test.h"
#include "interpreter_internal.h"

#define TEST_PARSE(src, exp)                                   \
  {                                                            \
    crisp_t *crisp = init_interpreter();                       \
    if (execute_crisp_code(crisp, src, exp, __FILE__, __LINE__,\
                         false, false, false) != PASS_CODE) {  \
      return FAIL_CODE;                                        \
    }                                                          \
  free_interpreter(crisp);                                     \
}

#define TEST_PARSE_FAILURE(src)                                \
  {                                                            \
    crisp_t *crisp = init_interpreter();                       \
    if (execute_crisp_code(crisp, src, "", __FILE__, __LINE__, \
                         true, false, false) != PASS_CODE) {   \
      return FAIL_CODE;                                        \
    }                                                          \
  free_interpreter(crisp);                                     \
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

  // Strings
  TEST_PARSE("\"one\"", "\"one\"");
  TEST_PARSE("(\"one\")", "(\"one\")");

  // Abbreviations
  TEST_PARSE("'a", "(quote a)");
  TEST_PARSE("'(1 2)", "(quote (1 2))");
  TEST_PARSE(",a", "(unquote a)");
  TEST_PARSE(",(1 2)", "(unquote (1 2))");
  TEST_PARSE("`a", "(quasiquote a)");
  TEST_PARSE("`(1 2)", "(quasiquote (1 2))");

  return PASS_CODE;
}
