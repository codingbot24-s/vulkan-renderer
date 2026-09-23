//
// Created by saad on 23/09/26.
//

#include "../log/loger.h"
#include "rmemory.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct memory_stats {
  uint64_t total_memory_allocation_size;
  uint64_t per_tag_allocation_size[MEM_TAG_MAX];
} memory_stats;

static memory_stats m_stats;

void zero_memory(void *block, size_t size) { memset(block, 0, size); }

void init_memory() { zero_memory(&m_stats, sizeof(m_stats)); }

void *r_allocate(size_t allocatation_size, MEMORY_TAGS mtag) {
  if (mtag == MEM_TAG_UNKNOWN) {
    R_WARN("allocation with memory tag unknown");
  }

  m_stats.total_memory_allocation_size += allocatation_size;
  m_stats.per_tag_allocation_size[mtag] += allocatation_size;

  void *block = malloc(allocatation_size);
  zero_memory(block, allocatation_size);

  return block;
}

void r_free(void *block, size_t size, MEMORY_TAGS mtag) {
  if (mtag == MEM_TAG_UNKNOWN) {
    R_WARN("free with memory tag unknown");
  }

  m_stats.per_tag_allocation_size[mtag] -= size;
  m_stats.total_memory_allocation_size -= size;

  free(block);
}

void *r_copy(void *dest, void *src, size_t size) {
  return memcpy(dest, src, size);
}
void *r_setmem(void *block, uint64_t value, size_t size) {
  return memset(block, value, size);
}
