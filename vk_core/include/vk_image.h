//
// Created by saad on 9/24/26.
//

#ifndef VULKAN_RENDERER_VK_IMAGE_H
#define VULKAN_RENDERER_VK_IMAGE_H
#include "../../renderer.h"

void create_image(renderer *renderer, uint32_t width, uint32_t height,
                  VkFormat format, VkImageTiling tiling,
                  VkImageUsageFlags usage_flags,
                  VkMemoryPropertyFlags properties, VkImage *p_to_store_image,
                  VkDeviceMemory *image_memory);

void create_image_views(renderer *renderer, VkImage image, VkFormat format,
                        VkImageAspectFlags aspect_flags,
                        VkImageView *p_to_image_view);

void transition_image_layout(renderer *renderer, VkImage image,
                             VkImageLayout old_layout, VkImageLayout new_layout,
                             VkAccessFlags2 src_access_mask,
                             VkAccessFlags2 dst_access_mask,
                             VkPipelineStageFlags2 src_stage_mask,
                             VkPipelineStageFlags2 dst_stage_mask,
                             VkImageAspectFlags aspect_flags);

#endif // VULKAN_RENDERER_VK_IMAGE_H
