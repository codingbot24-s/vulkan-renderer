//
// Created by saad on 9/26/26.
//

#include "include/vk_vertex_buffer.h"
#include "../log/loger.h"
#include "../memory/rmemory.h"
#include "../renderer.h"
#include "vulkan/vulkan_core.h"
#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

/// NOTE: we can use glmc also for math
typedef struct vertex {
  float pos[2];
  float color[3];

} vertex;

uint32_t find_memory_type(uint32_t memory_type,
                          VkMemoryPropertyFlags required_property_flags,
                          renderer *renderer) {
  VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
  vkGetPhysicalDeviceMemoryProperties(renderer->my_physical_device,
                                      &physical_device_memory_properties);

  const uint32_t physical_device_memory_type_count =
      physical_device_memory_properties.memoryTypeCount;

  for (uint32_t memory_index = 0;
       memory_index < physical_device_memory_type_count; ++memory_index) {
    const uint32_t memory_type_bits = (1 << memory_index);
    const bool is_required_memory_type = (memory_type & memory_type_bits);

    const VkMemoryPropertyFlags physical_device_memory_properties_flags =
        physical_device_memory_properties.memoryTypes[memory_index]
            .propertyFlags;
    const bool has_required_properties =
        (required_property_flags & physical_device_memory_properties_flags) ==
        required_property_flags;

    if (is_required_memory_type && has_required_properties) {
      return memory_index;
    }
  }

  return -1;
}

VkVertexInputBindingDescription get_binding_description() {
  return (VkVertexInputBindingDescription){.binding = 0,
                                           .stride = sizeof(vertex),
                                           .inputRate =
                                               VK_VERTEX_INPUT_RATE_VERTEX};
}

void get_attribute_description(
    VkVertexInputAttributeDescription *attribute_descriptions) {

  attribute_descriptions[0].location = 0;
  attribute_descriptions[0].binding = 0;
  attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[0].offset = 0;

  attribute_descriptions[1].location = 1;
  attribute_descriptions[1].binding = 0;
  attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[1].offset = sizeof(float) * 2;
}

void create_vertex_buffer(renderer *renderer) {
  const vertex vertices[sizeof(vertex) * 3] = {
      {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
      {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

  VkVertexInputAttributeDescription input_attribute_descriptions[2];
  get_attribute_description(input_attribute_descriptions);

  VkBufferCreateInfo buffer_create_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = NULL,
      .size = sizeof(vertices),
      .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  VkResult res = vkCreateBuffer(renderer->my_device, &buffer_create_info, NULL,
                                &renderer->vertex_buffer);

  if (res != VK_SUCCESS) {
    R_FATAL("cant create the vertex buffer");
  }

  VkMemoryRequirements buffer_memory_requirments;
  vkGetBufferMemoryRequirements(renderer->my_device, renderer->vertex_buffer,
                                &buffer_memory_requirments);

  VkMemoryAllocateInfo allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = NULL,
      .allocationSize = buffer_memory_requirments.size,
      .memoryTypeIndex =
          find_memory_type(buffer_memory_requirments.memoryTypeBits,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           renderer)};
  res = vkAllocateMemory(renderer->my_device, &allocate_info, NULL,
                         &renderer->vertex_buffer_memory);
  if (res != VK_SUCCESS) {
    R_FATAL("cant allocate the memory for vertex buffer in device");
  }

  res = vkBindBufferMemory(renderer->my_device, renderer->vertex_buffer,
                           renderer->vertex_buffer_memory, 0);
  if (res != VK_SUCCESS) {
    R_FATAL("cant bind the buffer memory");
  }

  void *data;
  res = vkMapMemory(renderer->my_device, renderer->vertex_buffer_memory, 0,
                    buffer_create_info.size, 0, &data);

  if (res != VK_SUCCESS) {
    R_FATAL("cant mapping memory to host");
  }
  r_copy(data, vertices, buffer_create_info.size);
  vkUnmapMemory(renderer->my_device, renderer->vertex_buffer_memory);

#ifndef NDEBUG
  R_INFO("vertex buffer created");
#endif
}

void create_index_buffer(renderer *renderer) {
  const uint16_t indices[6] = {0, 1, 2, 2, 3, 0};

  VkBufferCreateInfo index_buffer_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = NULL,
      .size = sizeof(indices),
      .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,

  };

  VkResult res = vkCreateBuffer(renderer->my_device, &index_buffer_info, NULL,
                                &renderer->index_buffer);
  if (res != VK_SUCCESS) {
    R_FATAL("cant create the index buffer");
  }

  VkMemoryRequirements index_buffer_memory_requirements;
  vkGetBufferMemoryRequirements(renderer->my_device, renderer->index_buffer,
                                &index_buffer_memory_requirements);

  VkMemoryAllocateInfo index_buffer_allocation_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      NULL,
      index_buffer_memory_requirements.size,

      find_memory_type(index_buffer_memory_requirements.memoryTypeBits,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       renderer)};

  res = vkAllocateMemory(renderer->my_device, &index_buffer_allocation_info,
                         NULL, &renderer->index_buffer_memory);

  if (res != VK_SUCCESS) {
    R_FATAL("cannot allocate memory for index buffer");
  }

  res = vkBindBufferMemory(renderer->my_device, renderer->index_buffer,
                           renderer->index_buffer_memory, 0);

  if (res != VK_SUCCESS) {
    R_FATAL("cannot bind the index buffer with memory");
  }

  void *mapped_memory;
  res = vkMapMemory(renderer->my_device, renderer->index_buffer_memory, 0,
                    index_buffer_info.size, 0, &mapped_memory);

  if (res != VK_SUCCESS) {
    R_FATAL("cannot map the memory");
  }
  r_copy(mapped_memory, (void *)indices, sizeof(indices));
  vkUnmapMemory(renderer->my_device, renderer->index_buffer_memory);

#ifndef NDEBUG
  R_INFO("index buffer created ");
#endif
}
