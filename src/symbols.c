#include "symbols.h"
#include "interpreter.h"

void reserved_symbols_init(crisp_t* crisp, symbols_t* symbols)
{
  symbols->quote = intern_string_null_terminated(crisp, "quote");
}
