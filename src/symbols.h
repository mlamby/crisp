#ifndef CRISP_SYMBOLS_H
#define CRISP_SYMBOLS_H

#include "common.h"

typedef enum {
  SYMBOL_QUOTE,

  SYMBOL_MAX_COUNT
} symbol_name_t;

typedef struct {
  const char* reserved_symbols[SYMBOL_MAX_COUNT];
} symbol_state_t;

void reserved_symbols_init(crisp_t* crisp, symbol_state_t* state);

bool is_named_keyword(symbol_state_t* state, const char* name, symbol_name_t symbol);

#endif //CRISP_SYMBOLS_H