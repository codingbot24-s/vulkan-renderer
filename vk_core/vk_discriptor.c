//
// Created by saad on 9/30/26.
//

#include "include/vk_discriptor.h"
#include "../constant.h"
#include "../log/loger.h"
#include "vulkan/vulkan_core.h"
#include <stdint.h>
#include <stdlib.h>

void create_descriptorset_layout(renderer *renderer) {

  const VkDescriptorSetLayoutBinding bindings[] = {
      (VkDescriptorSetLayoutBinding){
          .binding = 0,
          .descriptorCount = 1,
          .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
      },

      (VkDescriptorSetLayoutBinding){
          .binding = 1,
          .descriptorCount = 1,
          .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
          .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
      },
  };

  const VkDescriptorSetLayoutCreateInfo descriptor_layout_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pNext = NULL,
      .bindingCount = 2,
      .pBindings = bindings,

  };
  const VkResult res =
      vkCreateDescriptorSetLayout(renderer->my_device, &descriptor_layout_info,
                                  NULL, &renderer->descriptor_layout);
  if (res != VK_SUCCESS) {
    R_FATAL("cant create the descriptor layout");
  }

#ifndef NDEBUG
  R_INFO("descriptor layout created");
#endif
}

void create_descriptor_pool(renderer *renderer) {
  const VkDescriptorPoolSize poolsize_and_info[] = {
      (VkDescriptorPoolSize){
          .descriptorCount = MAX_FRAME_IN_FLIGHT,
          .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      },
      (VkDescriptorPoolSize){
          .descriptorCount = MAX_FRAME_IN_FLIGHT,
          .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      },
  };

  const VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .pNext = NULL,
      .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
      .maxSets = MAX_FRAME_IN_FLIGHT,
      .poolSizeCount = 2,
      .pPoolSizes = poolsize_and_info,
  };
  VkResult res =
      vkCreateDescriptorPool(renderer->my_device, &descriptor_pool_create_info,
                             NULL, &renderer->descriptor_pool);

  if (res != VK_SUCCESS) {
    R_FATAL("descriptor pool creating error");
  }
}

void create_descriptor_set(renderer *renderer) {

  VkDescriptorSetLayout layouts[2] = {renderer->descriptor_layout,
                                      renderer->descriptor_layout};

  VkDescriptorSet *descriptor_sets = malloc(sizeof(VkDescriptorSet) * 2);
  renderer->descriptor_sets = descriptor_sets;
  const VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .pNext = NULL,
      .descriptorPool = renderer->descriptor_pool,
      .descriptorSetCount = 2,
      .pSetLayouts = layouts};

  vkAllocateDescriptorSets(renderer->my_device, &descriptor_set_allocate_info,
                           renderer->descriptor_sets);

  /// NOTE: 2 is max frame in flight
  for (uint32_t i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
    const VkDescriptorBufferInfo buffer_info = {
        .buffer = renderer->uniform_buffers[i],
        .offset = 0,
        .range = VK_WHOLE_SIZE};

    const VkDescriptorImageInfo image_info = {
        .sampler = renderer->sampler,
        .imageView = renderer->texture_image_view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

    const VkWriteDescriptorSet write_descriptor_sets[] = {
        (VkWriteDescriptorSet){

            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = renderer->descriptor_sets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &buffer_info,
        },

        (VkWriteDescriptorSet){

            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = renderer->descriptor_sets[i],
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &image_info,
        },
     };

    vkUpdateDescriptorSets(renderer->my_device, 2, write_descriptor_sets, 0,
                           NULL);
  }

#ifndef NDEBUG
  R_INFO("descriptor set created");
#endif
}
