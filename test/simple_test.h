#ifndef SIMPLE_TEST_H
#define SIMPLE_TEST_H

#include "common.h"

#include <stdio.h>
#include <string.h>

#define FAIL_CODE 1
#define PASS_CODE 0

#define TEST_ASSERT(expr)                                           \
  if(!(expr)) {                                                     \
    printf("\n%s(%d): Test Fail: %s\n", __FILE__, __LINE__, #expr); \
    return FAIL_CODE;                                               \
  }

#define TEST_INT_EQ(exp1, exp2)                                     \
  if(!((exp1) == (exp2))) {                                         \
    printf("\n%s(%d): Test Fail\n", __FILE__, __LINE__);            \
    printf("  : %d != %d\n", (##exp1), (##exp2));                   \
    return FAIL_CODE;                                               \
  }

#define RUN_TEST(fn)                                                \
  if(fn() != PASS_CODE) {                                           \
    return FAIL_CODE;                                               \
  }

#define RUN_TEST_WITH_FIXTURE(fn) {                                 \
  test_fixture_t fixture;                                           \
  setup(&fixture);                                                  \
  if(fn(&fixture) != PASS_CODE) {                                   \
    teardown(&fixture);                                             \
    return FAIL_CODE;                                               \
  }                                                                 \
  teardown(&fixture);}

#endif

int execute_crisp_code(
  const char* src, 
  const char* expected, 
  const char* file_name, 
  int line, 
  bool expect_parse_fail,
  bool run_eval,
  bool expect_eval_fail);