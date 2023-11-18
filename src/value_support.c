#include "value_support.h"
#include "evaluator.h"

bool is_list(expr_t value)
{
  return (is_nil(value)) || (length(value) > 0);
}

size_t length(expr_t value)
{
  size_t sz = 0;
  expr_t c = value;
  while (true)
  {
    if (is_cons(c))
    {
      c = cdr(c);
      ++sz;
    }
    else if (is_nil(c))
      return sz;
    else
      break;
  }
  return 0;
}

list_iter_t iter_list(crisp_t *crisp, expr_t lst)
{
  if (!is_cons(lst))
  {
    crisp_eval_error(crisp, "Expected lst for iterations");
  }

  list_iter_t r = {
      .src = lst,
      .iter = NULL,
      .index = 0};

  return r;
}

expr_t iter_next(list_iter_t *i)
{
  if (i->iter == NULL)
  {
    // This is the first time next is called so just use the src
    i->iter = i->src;
  }
  else
  {
    // Move along the iterator
    i->iter = cdr(i->iter);
  }

  bool valid = !is_nil(i->iter);
  if (valid)
  {
    i->index++;
    return car(i->iter);
  }
  return NULL;
}
