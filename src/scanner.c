#include "scanner.h"

typedef struct
{
  const char *start;
  const char *current;
  int32_t line;
} scanner_t;

scanner_t scanner;

static bool is_at_end();
static bool is_digit(char c);
static bool is_letter(char c);
static bool is_delimiter(char c);
static bool is_whitespace(char c);
static bool is_symbol_initial(char c);
static bool is_symbol_subsequent(char c);
static token_t make_token(token_type_t type);
static token_t error_token(const char *message);
static token_t make_string();
static token_t make_digit();
static token_t make_identifier();
static char advance();
static bool match(char expected);
static char peek();
static void skip_whitespace();

void init_scanner(const char *source)
{
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
}

token_t scan_token()
{
  skip_whitespace();

  scanner.start = scanner.current;

  if (is_at_end())
    return make_token(TOKEN_EOF);

  char c = advance();

  switch (c)
  {
  case '(':
    return make_token(TOKEN_LEFT_PAREN);
  case ')':
    return make_token(TOKEN_RIGHT_PAREN);
  case '{':
    return make_token(TOKEN_LEFT_BRACE);
  case '}':
    return make_token(TOKEN_RIGHT_BRACE);
  case '[':
    return make_token(TOKEN_LEFT_BOX);
  case ']':
    return make_token(TOKEN_RIGHT_BOX);
  case '`':
    return make_token(TOKEN_BACKTICK);
  case '\'':
    return make_token(TOKEN_APOSTROPHE);
  case ',':
    return make_token(TOKEN_COMMA);
  case '.':
    return make_token(TOKEN_DOT);
  case '#':
    if (match('t') || match('T'))
    {
      return make_token(TOKEN_TRUE);
    }
    else if (match('f') || match('F'))
    {
      return make_token(TOKEN_FALSE);
    }
    return make_token(TOKEN_HASH);
  case '"':
    return make_string();
  case '-':
    if (is_delimiter(peek()))
    {
      return make_identifier();
    }
    return make_digit();
  case '+':
    if (is_delimiter(peek()))
    {
      return make_identifier();
    }
    return make_digit();
  }

  // Match positive numbers without a leading +/-
  if (is_digit(c))
  {
    return make_digit();
  }

  if (is_symbol_initial(c))
  {
    // Attempt to make an identifier
    return make_identifier();
  }

  return error_token("Unexpected character");
}

static bool is_at_end()
{
  return *scanner.current == '\0';
}

static bool is_digit(char c)
{
  return (c >= '0') && (c <= '9');
}

static bool is_letter(char c)
{
  return ((c >= 'a') && (c <= 'z')) ||
         ((c >= 'A') && (c <= 'Z'));
}

static bool is_whitespace(char c)
{
  bool result = false;

  switch (c)
  {
  case ' ':
  case '\r':
  case '\t':
  case '\n':
    result = true;
    break;
  }

  return result;
}

static bool is_symbol_initial(char c)
{
  return is_letter(c) ||
         // special initial
         (c == '!') ||
         (c == '$') ||
         (c == '%') ||
         (c == '&') ||
         (c == '*') ||
         (c == '/') ||
         (c == ':') ||
         (c == '<') ||
         (c == '=') ||
         (c == '>') ||
         (c == '?') ||
         (c == '~') ||
         (c == '_') ||
         (c == '^');
}

static bool is_symbol_subsequent(char c)
{
  return is_symbol_initial(c) ||
         is_digit(c) ||
         // Special subsequent characters
         (c == '.') ||
         (c == '+') ||
         (c == '@') ||
         (c == '-');
}

static bool is_delimiter(char c)
{
  return is_whitespace(c) ||
         (c == '(') ||
         (c == ')') ||
         (c == '[') ||
         (c == ']') ||
         (c == '"') ||
         (c == ';') ||
         (c == '#') ||
         (c == '\0');
}

static token_t make_token(token_type_t type)
{
  token_t token;
  token.type = type;
  token.start = scanner.start;
  token.length = (size_t)(scanner.current - scanner.start);
  token.line = scanner.line;
  return token;
}

static token_t error_token(const char *message)
{
  token_t token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (size_t)strlen(message);
  token.line = scanner.line;
  return token;
}

static char advance()
{
  char current = *(scanner.current);
  ++scanner.current;
  return current;
}

static bool match(char expected)
{
  if (is_at_end())
    return false;
  if (*scanner.current != expected)
    return false;
  scanner.current++;
  return true;
}

static char peek()
{
  return *scanner.current;
}

static void skip_whitespace()
{
  bool keep_looping = true;

  while (keep_looping)
  {
    char c = peek();
    switch (c)
    {
    case ' ':
    case '\r':
    case '\t':
      advance();
      break;
    case '\n':
      scanner.line++;
      advance();
      break;
    case ';':
      // Treat comments as whitespace
      // A comment goes until the end of the line.
      while (peek() != '\n' && !is_at_end())
      {
        advance();
      }
      break;
    default:
      keep_looping = false;
    }
  }
}

static token_t make_string()
{
  bool keep_looping = true;
  bool escaped = false;
  bool found_end = false;

  while (keep_looping)
  {
    if (is_at_end())
    {
      keep_looping = false;
    }
    else
    {
      if (escaped)
      {
        // Escaped so unconditionally accept the next character
        escaped = false;
      }
      else if (peek() == '"')
      {
        // Found an unescaped end of string
        keep_looping = false;
        found_end = true;
      }
      else if (peek() == '\\')
      {
        // Found an unescaped escape character
        escaped = true;
      }

      // In all cases accept the next character
      advance();
    }
  }

  if (!found_end)
    return error_token("Unterminated string.");

  return make_token(TOKEN_STRING);
}

static token_t make_digit()
{
  // Currently only handles integers
  while (is_digit(peek()))
  {
    advance();
  }

  if (!is_delimiter(peek()))
  {
    return error_token("digit incorrectly delimited");
  }

  return make_token(TOKEN_NUMBER);
}

static token_t make_identifier()
{
  while (is_symbol_subsequent(peek()))
  {
    advance();
  }

  if (!is_delimiter(peek()))
  {
    return error_token("symbol incorrectly delimited");
  }

  return make_token(TOKEN_IDENTIFIER);
}
