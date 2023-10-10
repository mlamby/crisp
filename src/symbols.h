#ifndef CRISP_SYMBOLS_H
#define CRISP_SYMBOLS_H

#include "common.h"

typedef struct {
  const char* quote;
} symbols_t;

void reserved_symbols_init(crisp_t* crisp, symbols_t* state);

#endif //CRISP_SYMBOLS_H
