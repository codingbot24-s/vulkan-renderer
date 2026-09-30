//
// Created by saad on 9/30/26.
//

#include "include/vk_discriptor.h"
#include "../libs/cglm/include/cglm/mat4.h"
#include "../log/loger.h"
#include "vulkan/vulkan_core.h"

typedef struct uniform_buffer_obj {
  mat4 model;
  mat4 view;
  mat4 proj;
} uniform_buffer_obj;

void create_descriptorset_layout(renderer *renderer) {
  VkDescriptorSetLayoutBinding ubo_layout_bindings = {
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,

  };
  VkDescriptorSetLayoutCreateInfo descriptor_layout_create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pNext = NULL,
      .bindingCount = 1,
      .pBindings = &ubo_layout_bindings,
  };

  VkResult res = vkCreateDescriptorSetLayout(
      renderer->my_device, &descriptor_layout_create_info, NULL,
      &renderer->descriptor_layout);

  if (res != VK_SUCCESS) {
    R_FATAL("descriptor layout creating error");
  }
}
