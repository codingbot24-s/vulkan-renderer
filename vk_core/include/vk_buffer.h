//
// Created by saad on 9/26/26.
//

#ifndef VULKAN_RENDERER_VK_VERTEX_BUFFER_H
#define VULKAN_RENDERER_VK_VERTEX_BUFFER_H

#include "../../renderer.h"
#include <vulkan/vulkan_core.h>

VkResult create_buffer(renderer *renderer, VkDeviceSize buffer_size,
                       VkBufferUsageFlags usage_of_buffer,
                       VkSharingMode buffer_sharing_mode,
                       VkBuffer *p_to_store_buffer);
VkResult allocate_buffer(renderer *renderer, uint32_t memory_type_index,
                         VkDeviceSize size_of_allocation,
                         VkDeviceMemory *p_to_store_buffer_memory);
VkResult bind_buffer_memory(renderer *renderer, VkBuffer buffer_to_bind,
                            VkDeviceMemory mem_to_bind_with,
                            VkDeviceSize offset);
uint32_t find_memory_type(uint32_t memory_type,
                          VkMemoryPropertyFlags required_property_flags,
                          renderer *renderer);
VkResult map_memory(renderer *renderer, VkDeviceMemory device_memory,
                    VkDeviceSize offset, VkDeviceSize buffer_size, void **data);

#endif // VULKAN_RENDERER_VK_VERTEX_BUFFER_H
