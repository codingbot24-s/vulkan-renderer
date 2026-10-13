//
// Created by saad on 9/24/26.
//

#include "./include/vk_image.h"
#include "../log/loger.h"
#include "include/vk_buffer.h"
#include "vulkan/vulkan_core.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

void create_image(renderer *renderer, uint32_t width, uint32_t height,
                  VkFormat format, VkImageTiling tiling,
                  VkImageUsageFlags usage_flags,
                  VkMemoryPropertyFlags properties, VkImage *p_to_store_image,
                  VkDeviceMemory *image_memory) {

  VkImageCreateInfo image_creatiion_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .pNext = NULL,
      .imageType = VK_IMAGE_TYPE_2D,
      .format = format,
      .extent = {width, height, 1},
      .tiling = tiling,
      .usage = usage_flags,
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = VK_SAMPLE_COUNT_1_BIT,
  };

  VkResult res = vkCreateImage(renderer->my_device, &image_creatiion_info, NULL,
                               p_to_store_image);
  if (res != VK_SUCCESS) {
    R_FATAL("error creating image");
  }
  VkMemoryRequirements image_memory_requirments;
  vkGetImageMemoryRequirements(renderer->my_device, *p_to_store_image,
                               &image_memory_requirments);

  VkMemoryAllocateInfo alloaction_info_for_image = {
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = NULL,
      .allocationSize = image_memory_requirments.size,
      find_memory_type(image_memory_requirments.memoryTypeBits, properties,
                       renderer)};
  res = vkAllocateMemory(renderer->my_device, &alloaction_info_for_image, NULL,
                         image_memory);

  if (res != VK_SUCCESS) {
    R_FATAL("error allocating memory for image");
  }
  res = vkBindImageMemory(renderer->my_device, *p_to_store_image, *image_memory,

                          0);

  if (res != VK_SUCCESS) {
    R_FATAL("error binding image memory");
  }
}

void create_image_views(renderer *renderer, VkImage image, VkFormat format,
                        VkImageAspectFlags aspect_flags,
                        VkImageView *p_to_image_view) {
  VkImageViewCreateInfo image_view_info = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .image = image,
      .format = format,
      .subresourceRange =
          {
              .aspectMask = aspect_flags,
              .layerCount = 1,
              .levelCount = 1,
          },

  };

  VkResult res = vkCreateImageView(renderer->my_device, &image_view_info, NULL,
                                   p_to_image_view);

  if (res != VK_SUCCESS) {
    R_FATAL("error creating the image view");
  }
}

/// NOTE: from now on this function will require the command buffer for pipeline
/// barrier

void transition_image_layout(renderer *renderer, VkImage image,
                             VkCommandBuffer *cmd_buffer,
                             VkImageLayout old_layout, VkImageLayout new_layout,
                             VkAccessFlags2 src_access_mask,
                             VkAccessFlags2 dst_access_mask,
                             VkPipelineStageFlags2 src_stage_mask,
                             VkPipelineStageFlags2 dst_stage_mask,
                             VkImageAspectFlags aspect_flags) {
  /// NOTE: check our other call to this function so this check dont create the
  /// problem for us
  if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
      new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    src_access_mask = 0;
    src_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;

    dst_stage_mask = VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT;
    dst_access_mask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
  } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    src_access_mask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    src_stage_mask = VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT;

    dst_stage_mask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
    dst_access_mask = VK_ACCESS_2_SHADER_READ_BIT;
  }
  VkImageMemoryBarrier2 barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
      .srcStageMask = src_stage_mask,
      .srcAccessMask = src_access_mask,
      .dstStageMask = dst_stage_mask,
      .dstAccessMask = dst_access_mask,
      .oldLayout = old_layout,
      .newLayout = new_layout,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = image,
      .subresourceRange =
          {
              .aspectMask = aspect_flags,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
          }

  };

  VkDependencyInfo dependency_info = {
      .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
      .imageMemoryBarrierCount = 1,
      .pImageMemoryBarriers = &barrier,
  };

  vkCmdPipelineBarrier2(*cmd_buffer, &dependency_info);
}
