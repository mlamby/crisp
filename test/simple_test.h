#ifndef SIMPLE_TEST_H
#define SIMPLE_TEST_H

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

#endif