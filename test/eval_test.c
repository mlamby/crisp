#include "simple_test.h"

#define TEST_EVAL(src, exp)                                    \
  if (execute_crisp_code(src, exp, __FILE__, __LINE__,         \
                         false, true, false) != PASS_CODE) {   \
    return FAIL_CODE;                                          \
  }

#define TEST_EVAL_FAILURE(src)                                 \
  if (execute_crisp_code(src, "", __FILE__, __LINE__,          \
                         false, true, true) != PASS_CODE) {    \
    return FAIL_CODE;                                          \
  }

int test_builtin_type_evaluation();
int test_math_evaluation();

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  RUN_TEST(test_builtin_type_evaluation);
  RUN_TEST(test_math_evaluation);

  return PASS_CODE;
}

int test_builtin_type_evaluation()
{
  // Numbers
  TEST_EVAL("5", "5");
  TEST_EVAL("'5", "5");
  TEST_EVAL("(list? 5)", "false");
  TEST_EVAL("(boolean? 5)", "false");
  TEST_EVAL("(symbol? 5)", "false");
  TEST_EVAL("(number? 5)", "true");
  TEST_EVAL("(string? 5)", "false");
  TEST_EVAL("(not 5)", "false");
  TEST_EVAL_FAILURE("(length 5)");

  // Strings
  TEST_EVAL("\"one\"", "\"one\"");
  TEST_EVAL("'\"one\"", "\"one\"");

  // Symbols
  TEST_EVAL_FAILURE("one"); // symbol one is not defined
  TEST_EVAL("'one", "one"); // quoted
  TEST_EVAL("(list? 'one)", "false");
  TEST_EVAL("(boolean? 'one)", "false");
  TEST_EVAL("(symbol? 'one)", "true");
  TEST_EVAL("(number? 'one)", "false");
  TEST_EVAL("(string? 'one)", "false");
  TEST_EVAL("(not 'one)", "false");
  TEST_EVAL_FAILURE("(length 'one)");

  // Lists
  TEST_EVAL("()", "()");
  TEST_EVAL("'()", "()");
  TEST_EVAL("(list? ())", "true");
  TEST_EVAL("(list? '(a b c))", "true");
  TEST_EVAL("(boolean? ())", "false");
  TEST_EVAL("(symbol? ())", "false");
  TEST_EVAL("(number? ())", "false");
  TEST_EVAL("(string? ())", "false");
  TEST_EVAL("(not ())", "false");
  TEST_EVAL("(length ())", "0");
  TEST_EVAL("(length '(1 2))", "2");
  TEST_EVAL("(length '('one 2 \"three\"))", "3");
  TEST_EVAL("(list)","()");
  TEST_EVAL("(list 1 2 3)", "(1 2 3)");

  // Improper lists
  TEST_EVAL("(list? '(a b . c))", "false");
  TEST_EVAL_FAILURE("(length '(1 2 . 3))");

  return PASS_CODE;
}

int test_math_evaluation()
{  
  // math functions
  TEST_EVAL("(+ 1 2)", "3");
  TEST_EVAL("(- 5 1)", "4");
  TEST_EVAL("(/ 5 2)", "2.5");
  TEST_EVAL("(* 5 2)", "10");

  // Tests from mal
  // https://github.com/kanaka/mal/blob/master/impls/tests/step2_eval.mal
  TEST_EVAL("(+ 1 2)", "3");
  TEST_EVAL("(+ 5 (* 2 3))", "11");
  TEST_EVAL("(- (+ 5 (* 2 3)) 3)", "8");
  TEST_EVAL("(/ (- (+ 5 (* 2 3)) 3) 4)", "2");
  TEST_EVAL("(/ (- (+ 515 (* 87 311)) 302) 27)", "1010");
  TEST_EVAL("(* -3 6)", "-18");
  TEST_EVAL("(/ (- (+ 515 (* -87 311)) 296) 27)", "-994");

  return PASS_CODE;
}
