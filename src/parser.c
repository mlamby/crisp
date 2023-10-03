#include "parser.h"
#include "value.h"
#include "scanner.h"
#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

typedef enum
{
  BRACKET_TYPE_PAREN,
  BRACKET_TYPE_BOX
} bracket_type_t;

// Parse functions return true on success
static expr_t parse_form(crisp_t *crisp, token_t next);
static expr_t parse_list(crisp_t *crisp, bracket_type_t bt);
static expr_t parse_symbol_atom(crisp_t *crisp, token_t token);
static expr_t parse_string_atom(crisp_t *crisp, token_t token);
static expr_t parse_number_atom(crisp_t *crisp, token_t token);
static expr_t parse_abbreviation(crisp_t *crisp, const char *symbol);
static bool is_list_end(token_type_t tt, bracket_type_t bt);

// Error handling functions
static void errorAt(crisp_t *, token_t *token, const char *message);

static bool parse_error = false;

expr_t parse(crisp_t *crisp, const char *source)
{
  init_scanner(source);
  parse_error = false;
  expr_t result = parse_form(crisp, scan_token());
  if (parse_error)
  {
    return NULL;
  }

  if (result == NULL)
  {
    fprintf(stderr, "Error: parse_form returned NULL\n");
  }

  return result;
}

static expr_t parse_form(crisp_t *crisp, token_t next)
{
  expr_t result = NULL;

  switch (next.type)
  {
  case TOKEN_LEFT_PAREN:
    result = parse_list(crisp, BRACKET_TYPE_PAREN);
    break;

  case TOKEN_LEFT_BOX:
    result = parse_list(crisp, BRACKET_TYPE_BOX);
    break;

  case TOKEN_STRING:
    result = parse_string_atom(crisp, next);
    break;

  case TOKEN_NUMBER:
    result = parse_number_atom(crisp, next);
    break;

  case TOKEN_IDENTIFIER:
    result = parse_symbol_atom(crisp, next);
    break;

  case TOKEN_TRUE:
    result = bool_value(true);
    break;

  case TOKEN_FALSE:
    result = bool_value(false);
    break;

  case TOKEN_APOSTROPHE:
    result = parse_abbreviation(crisp, "quote");
    break;

  case TOKEN_COMMA:
    result = parse_abbreviation(crisp, "unquote");
    break;

  case TOKEN_BACKTICK:
    result = parse_abbreviation(crisp, "quasiquote");
    break;

  case TOKEN_ERROR:
    errorAt(crisp, &next, "");
    break;

  default:
    errorAt(crisp, &next, "Unexpected token");
  }

  return result;
}

static expr_t parse_list(crisp_t *crisp, bracket_type_t bt)
{
  expr_t result = NULL;
  bool dot_list = false;

  token_t next = scan_token();

  if (next.type == TOKEN_DOT)
  {
    next = scan_token();
    dot_list = true;
  }

  if (next.type == TOKEN_EOF)
  {
    errorAt(crisp, &next, "Eof found whilst parsing list");
    return NULL;
  }

  if (is_list_end(next.type, bt))
  {
    if (dot_list)
    {
      errorAt(crisp, &next, "Expecting a single datum after '.'");
      return NULL;
    }
    else
    {
      return nil_value();
    }
  }

  result = parse_form(crisp, next);
  if (result == NULL)
  {
    return NULL;
  }

  if (dot_list)
  {
    // value in result should be the last value in the list.
    // ensure that there is a trailing bracket
    next = scan_token();
    if (!is_list_end(next.type, bt))
    {
      errorAt(crisp, &next, "Expecting only one datum after '.'");
      return NULL;
    }

    // return the result of parsing the form directly.
    // since a dot can only preceed the last item in a list
    // returning it directly will mean that the caller
    // will cons it into the cdr position.
  }
  else
  {
    // Return a normal list form.
    result = cons(result, parse_list(crisp, bt));
  }

  return result;
}

static expr_t parse_symbol_atom(crisp_t *crisp, token_t token)
{
  return atom_value(crisp, token.start, token.length);
}

static expr_t parse_string_atom(crisp_t *crisp, token_t token)
{
  // the token is the fully quoted string, e.g. "hello".
  // skip the quotes when copying.
  return string_value(crisp, token.start + 1, token.length - 2);
}

static expr_t parse_number_atom(crisp_t *crisp, token_t token)
{
  double value = strtod(token.start, NULL);
  if (errno == ERANGE)
  {
    errno = 0;
    errorAt(crisp, &token, "Malformed number");
    return NULL;
  }

  return number_value(value);
}

static expr_t parse_abbreviation(crisp_t *crisp, const char *symbol)
{
  return cons(
      atom_value_null_terminated(crisp, symbol),
      cons(
          parse_form(crisp, scan_token()),
          nil_value()));
}

static bool is_list_end(token_type_t tt, bracket_type_t bt)
{
  if (bt == BRACKET_TYPE_PAREN)
  {
    return (tt == TOKEN_RIGHT_PAREN);
  }
  else if (bt == BRACKET_TYPE_BOX)
  {
    return (tt == TOKEN_RIGHT_BOX);
  }
  return false;
}

static void errorAt(crisp_t *crisp, token_t *token, const char *message)
{
  (void)crisp;
  parse_error = true;
  fprintf(stderr, "[line %d] Error", token->line);

  if (token->type == TOKEN_EOF)
  {
    fprintf(stderr, " at end");
  }
  else if (token->type == TOKEN_ERROR)
  {
    fprintf(stderr, " - '%.*s'", token->length, token->start);
  }
  else
  {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, ": %s\n", message);
}