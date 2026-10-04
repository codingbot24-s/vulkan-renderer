//
// Created by saad on 10/2/26.
//
#include "../log/loger.h"
#include "../memory/rmemory.h"
#include "include/vk_buffer.h"
#include <stdint.h>

void create_index_buffer(renderer *renderer) {
  const uint16_t indices[6] = {0, 1, 2, 2, 3, 0};

  VkResult res =
      create_buffer(renderer, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                    VK_SHARING_MODE_EXCLUSIVE, &renderer->index_buffer);
  if (res != VK_SUCCESS) {
    R_FATAL("cant create the vertex buffer");
  }

  VkMemoryRequirements index_buffer_memory_requirements;
  vkGetBufferMemoryRequirements(renderer->my_device, renderer->index_buffer,
                                &index_buffer_memory_requirements);

  res = allocate_buffer(
      renderer,
      find_memory_type(index_buffer_memory_requirements.memoryTypeBits,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       renderer),
      index_buffer_memory_requirements.size, &renderer->index_buffer_memory);

  if (res != VK_SUCCESS) {
    R_FATAL("cannot allocate memory for index buffer");
  }

  res = bind_buffer_memory(renderer, renderer->index_buffer,
                           renderer->index_buffer_memory, 0);

  if (res != VK_SUCCESS) {
    R_FATAL("cannot bind the index buffer with memory");
  }

  void *mapped_memory;
  res = map_memory(renderer, renderer->index_buffer_memory, 0,
                   index_buffer_memory_requirements.size, &mapped_memory);

  if (res != VK_SUCCESS) {
    R_FATAL("cannot map the memory");
  }
  r_copy(mapped_memory, (void *)indices, sizeof(indices));
  vkUnmapMemory(renderer->my_device, renderer->index_buffer_memory);

#ifndef NDEBUG
  R_INFO("index buffer created ");
#endif
}
