//
// Created by saad on 9/26/26.
//

#include "include/vk_buffer.h"
#include "../log/loger.h"
#include "../renderer.h"
#include "vulkan/vulkan_core.h"
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>

/// TODO: Use cglm for this struct fields
/// TODO: descriptor and descriptor pool creation info into diffrent files
/// TODO: Remove repeated logic from this file
/// TODO: Define the Max frame in flight and remove number 2 from there
VkResult create_buffer(renderer *renderer, VkDeviceSize buffer_size,
                       VkBufferUsageFlags usage_of_buffer,
                       VkSharingMode buffer_sharing_mode,
                       VkBuffer *p_to_store_buffer) {

  VkBufferCreateInfo buffer_creation_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .size = buffer_size,
      .usage = usage_of_buffer,
      .sharingMode = buffer_sharing_mode,
  };

  VkResult res = vkCreateBuffer(renderer->my_device, &buffer_creation_info,
                                NULL, p_to_store_buffer);
  if (res != VK_SUCCESS) {
    R_FATAL("error creating buffer");
  }

  return res;
}

VkResult allocate_buffer(renderer *renderer, uint32_t memory_type_index,
                         VkDeviceSize size_of_allocation,
                         VkDeviceMemory *p_to_store_buffer_memory) {

  VkResult res;
  const VkMemoryAllocateInfo allocate_info = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .memoryTypeIndex = memory_type_index,
      .allocationSize = size_of_allocation,
      .pNext = NULL,
  };

  res = vkAllocateMemory(renderer->my_device, &allocate_info, NULL,
                         p_to_store_buffer_memory);
  if (res != VK_SUCCESS) {
    R_FATAL("error allocating buffer");
  }

  return res;
}

VkResult bind_buffer_memory(renderer *renderer, VkBuffer buffer_to_bind,
                            VkDeviceMemory mem_to_bind_with,
                            VkDeviceSize offset) {
  VkResult res;
  res = vkBindBufferMemory(renderer->my_device, buffer_to_bind,
                           mem_to_bind_with, offset);
  if (res != VK_SUCCESS) {
    R_FATAL("error binding buffer");
  }

  return res;
}

VkResult map_memory(renderer *renderer, VkDeviceMemory device_memory,
                    VkDeviceSize offset, VkDeviceSize buffer_size,
                    void **data) {

  VkResult res = vkMapMemory(renderer->my_device, device_memory, offset,
                             buffer_size, 0, data);

  if (res != VK_SUCCESS) {
    R_FATAL("error mapping to host");
  }

  return res;
}
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






