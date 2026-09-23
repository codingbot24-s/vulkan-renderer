//
// Created by saad on 23/09/26.
//

#ifndef VULKAN_RENDERER_RMEMORY_H
#define VULKAN_RENDERER_RMEMORY_H

#include <stddef.h>
#include <stdint.h>
typedef enum MEMORY_TAGS {
  MEM_TAG_UNKNOWN,
  MEM_TAG_DARRAY,
  MEM_TAG_STRING,
  MEM_TAG_RENDERER,

  MEM_TAG_MAX,
} MEMORY_TAGS;

// set the given block two zero
void zero_memory(void *block, size_t size);
void *r_allocate(size_t allocatation_size, MEMORY_TAGS mtag);
void r_free(void *block, size_t size, MEMORY_TAGS mtag);
void *r_copy(void *dest, void *src, size_t size);
void *r_setmem(void *block, uint64_t value, size_t size);

#endif // VULKAN_RENDERER_RMEMORY_H
