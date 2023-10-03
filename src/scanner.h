#ifndef SCANNER_H
#define SCANNER_H

#include "common.h"

typedef enum {
  // Single-character tokens.
  TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
  TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
  TOKEN_LEFT_BOX, TOKEN_RIGHT_BOX,
  TOKEN_BACKTICK, TOKEN_APOSTROPHE,
  TOKEN_COMMA, TOKEN_DOT,
  
  // One or two character tokens.
  TOKEN_HASH, TOKEN_TRUE, TOKEN_FALSE,

  // Literals.
  TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

  TOKEN_ERROR, TOKEN_EOF
} token_type_t;


typedef struct {
  token_type_t type;
  const char* start;
  int length;
  int line;
} token_t;

void init_scanner(const char* source);
token_t scan_token();

#endif
