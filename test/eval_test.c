#include "simple_test.h"
#include "interpreter_internal.h"

#define TEST_EVAL(src, exp)                                    \
  if (execute_crisp_code(fixture->crisp, src, exp,             \
                        __FILE__, __LINE__,                    \
                        false, true, false) != PASS_CODE) {    \
    return FAIL_CODE;                                          \
  }

#define TEST_EVAL_FAILURE(src)                                 \
  if (execute_crisp_code(fixture->crisp, src, "",              \
                        __FILE__, __LINE__,                    \
                        false, true, true) != PASS_CODE) {     \
    return FAIL_CODE;                                          \
  }

typedef struct
{
  crisp_t *crisp;
} test_fixture_t;

static void setup(test_fixture_t *fixture);
static void teardown(test_fixture_t *fixture);

int test_builtin_type_evaluation(test_fixture_t *fixture);
int test_math_evaluation(test_fixture_t *fixture);
int test_lambda_evaluation(test_fixture_t *fixture);
int test_top_level_defines(test_fixture_t *fixture);

int main(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  RUN_TEST_WITH_FIXTURE(test_builtin_type_evaluation);
  RUN_TEST_WITH_FIXTURE(test_math_evaluation);
  RUN_TEST_WITH_FIXTURE(test_lambda_evaluation);
  RUN_TEST_WITH_FIXTURE(test_top_level_defines);

  return PASS_CODE;
}

int test_builtin_type_evaluation(test_fixture_t *fixture)
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
  TEST_EVAL("(list 'a 'b 'c)", "(a b c)");
  TEST_EVAL("(list 'a)", "(a)");
  TEST_EVAL("(list)", "()");
  TEST_EVAL("(list 1 2 3)", "(1 2 3)");

  // Improper lists
  TEST_EVAL("(list? '(a b . c))", "false");
  TEST_EVAL_FAILURE("(length '(1 2 . 3))");

  return PASS_CODE;
}

int test_math_evaluation(test_fixture_t *fixture)
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

int test_lambda_evaluation(test_fixture_t *fixture)
{
  // Tests from The Scheme Programming Language
  // https://www.scheme.com/tspl4/binding.htm
  TEST_EVAL("((lambda (x) (+ x 3)) 7)", "10");
  TEST_EVAL("((lambda (x) (+ x 3) (+ x 4)) 7)", "11"); // multiple lambda bodies
  TEST_EVAL("((lambda (x y) (* x (+ x y))) 7 13)", "140");
  TEST_EVAL("((lambda (f x) (f x x)) + 11)", "22");
  TEST_EVAL("((lambda () (+ 3 4)))", "7");

  TEST_EVAL("((lambda (x . y) (list x y)) 28 37)", "(28 (37))");
  TEST_EVAL("((lambda (x . y) (list x y)) 28 37 47 28)", "(28 (37 47 28))");
  TEST_EVAL("((lambda (x y . z) (list x y z)) 1 2 3 4)", "(1 2 (3 4))");
  TEST_EVAL("((lambda x x) 7 13)", "(7 13)");

  return PASS_CODE;
}

int test_top_level_defines(test_fixture_t *fixture)
{
  // Tests from The Scheme Programming Language
  // https://www.scheme.com/tspl4/binding.html
  TEST_EVAL("(define x 5)", "()");
  TEST_EVAL("x", "5");

  TEST_EVAL("(define f (lambda (x y) (* (+ x y) 2)))", "()");
  TEST_EVAL("(f 5 4)", "18");

  TEST_EVAL("(define sum-of-squares (lambda (x y) (+ (* x x) (* y y))))", "()");
  TEST_EVAL("(sum-of-squares 5 4)", "41");

  // Abbreviated lambdas are not supported.

  return PASS_CODE;
}

static void setup(test_fixture_t *fixture)
{
  fixture->crisp = init_interpreter();
}

static void teardown(test_fixture_t *fixture)
{
  free_interpreter(fixture->crisp);
}
