#include "memory.h"

#include <stdlib.h>
#include <stdio.h>

// Linked list structure, used to keep track of allocated memory
typedef struct memory_header_t memory_header_t;
struct memory_header_t
{
  memory_header_t *prev;
  memory_header_t *next;
  size_t sz;
  const char *file;
  unsigned int line;
};

// The head of the linked list that tracks memory usage.
static memory_header_t *memory_head = NULL;

// File pointer where memory is logged.
// If the file pointer is null then memory logging is disabled.
static FILE *memory_file = NULL;

#define HEADER_SIZE sizeof(memory_header_t)
#define ALLOC_SIZE(sz) (sz + HEADER_SIZE)
#define SKIP_HEADER(p) ((void *)((char *)p + HEADER_SIZE))
#define HEADER_ADDRESS(p) ((memory_header_t *)((char *)p - HEADER_SIZE))

static bool is_memory_logging_enabled();
static void memory_end_logging(void);
static void tstore(memory_header_t *hdr, size_t sz, const char *file, unsigned int line);
static void tfree(memory_header_t *hdr, size_t sz, const char *file, unsigned int line);

void *reallocate(void *pointer, size_t old_size, size_t new_size, const char *file, unsigned int line)
{
  if (new_size == 0)
  {
    if (old_size == 0)
      return NULL;

    if (is_memory_logging_enabled())
    {
      tfree(HEADER_ADDRESS(pointer), old_size, file, line);
      free(HEADER_ADDRESS(pointer));
    }
    else
    {
      free(pointer);
    }
    return NULL;
  }

  if (pointer != NULL)
  {
    fprintf(stderr, "\nReallocation not supported!\n");
    exit(1);
  }

  void *result = NULL;

  if (is_memory_logging_enabled())
  {
    memory_header_t *hdr = malloc(ALLOC_SIZE(new_size));
    if (hdr != NULL)
    {
      tstore(hdr, new_size, file, line);
      result = SKIP_HEADER(hdr);
    }
  }
  else
  {
    result = malloc(new_size);
  }

  if (result == NULL)
  {
    fprintf(stderr, "\nOut of memory!\n");
    exit(1);
  }

  return result;
}

void memory_install_logging(const char *file_name)
{
  memory_file = fopen(file_name, "w");
  atexit(memory_end_logging);
}

void memory_dump()
{
  if (is_memory_logging_enabled())
  {
    memory_header_t *node = memory_head;
    size_t count = 0;
    fprintf(memory_file, "\nMemory Block Dump:\n");
    while (node != NULL)
    {
      fprintf(memory_file, "  %s:%d %p (sz=%zu)\n", node->file, node->line, (void *)node, node->sz);
      node = node->next;
      count++;
    }
    fprintf(memory_file, "  Count: %zu\n", count);
  }
}

static bool is_memory_logging_enabled()
{
  return memory_file != NULL;
}

static void memory_end_logging(void)
{
  if (memory_file != NULL)
  {
    memory_dump();
    fclose(memory_file);
    memory_file = NULL;
  }
}

static void tstore(memory_header_t *hdr, size_t sz, const char *file, unsigned int line)
{
  fprintf(memory_file, "Malloc: %s:%d %p (sz=%zu)\n", file, line, (void *)hdr, sz);
  fflush(memory_file);

  hdr->file = file;
  hdr->line = line;
  hdr->sz = sz;
  hdr->next = NULL;
  hdr->prev = NULL;

  if (memory_head != NULL)
  {
    memory_head->prev = hdr;
    hdr->next = memory_head;
  }

  memory_head = hdr;
}

static void tfree(memory_header_t *hdr, size_t sz, const char *file, unsigned int line)
{
  fprintf(memory_file, "Free: %s:%d %p (sz=%zu)\n", file, line, (void *)hdr, sz);
  fflush(memory_file);

  if (hdr->prev != NULL)
  {
    hdr->prev->next = hdr->next;
  }

  if (hdr->next != NULL)
  {
    hdr->next->prev = hdr->prev;
  }

  if (hdr == memory_head)
  {
    memory_head = hdr->next;
  }
}
