#include "simple_test.h"

#include "scanner.h"

#define MAX_TOKENS 1000
static token_t tokens[MAX_TOKENS] = {0};
static size_t token_count = 0;
static size_t token_index = 0;

#define NT()  tokens[token_index++]
#define NTT() NT().type

void run_scanner(const char* source)
{
  init_scanner(source);
  token_index = 0;

  while(true)
  {
    token_t next = scan_token();
    tokens[token_count++] = next;
    if(next.type == TOKEN_EOF) break;
  }
}

int main(int argc, char** argv)
{
  (void)argc;
  (void)argv;
  {
    run_scanner("(+ \"abc\")");
    TEST_ASSERT(NTT() == TOKEN_LEFT_PAREN);
    TEST_ASSERT(NTT() == TOKEN_IDENTIFIER);
    TEST_ASSERT(NTT() == TOKEN_STRING);
    TEST_ASSERT(NTT() == TOKEN_RIGHT_PAREN);
    TEST_ASSERT(NTT() == TOKEN_EOF);
  }

  return PASS_CODE;
}
