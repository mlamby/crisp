#include "symbols.h"
#include "interpreter.h"

void reserved_symbols_init(crisp_t* crisp, symbol_state_t* state)
{
  state->reserved_symbols[SYMBOL_QUOTE] = intern_string_null_terminated(crisp, "quote");
}

bool is_named_keyword(symbol_state_t* state, const char* name, symbol_name_t symbol)
{
  return (state->reserved_symbols[symbol] == name);
}