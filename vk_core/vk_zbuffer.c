//
// Created by saad on 10/5/26.
//

#include "../log/loger.h"
#include "../renderer.h"
#include "./include/vk_image.h"
#include "vulkan/vulkan_core.h"
#include <stdint.h>
#include <stdlib.h>

VkFormat find_suported_format_for_Zimage(VkFormat *formats,
                                         uint32_t format_count,
                                         VkImageTiling image_tiling,
                                         VkFormatFeatureFlags features,
                                         renderer *renderer) {

  VkFormatProperties device_format_properties = {0};
  for (int i = 0; i < format_count; ++i) {
    vkGetPhysicalDeviceFormatProperties(renderer->my_physical_device,
                                        formats[i], &device_format_properties);
    if (((image_tiling == VK_IMAGE_TILING_LINEAR) &&
         ((device_format_properties.linearTilingFeatures & features) ==
          features)) ||
        ((image_tiling == VK_IMAGE_TILING_OPTIMAL) &&
         ((device_format_properties.optimalTilingFeatures & features) ==
          features))) {

      return formats[i];
    }
  }
  R_FATAL("failed to find supported format");
}

VkFormat get_depth_format(renderer *renderer) {
  VkFormat formats[3] = {
      VK_FORMAT_D32_SFLOAT,
      VK_FORMAT_D24_UNORM_S8_UINT,
      VK_FORMAT_D32_SFLOAT_S8_UINT,
  };
  VkFormat supported_format = find_suported_format_for_Zimage(
      formats, 3, VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, renderer);

  return supported_format;
}

void create_depth_resource(renderer *renderer) {

  VkFormat depth_format = get_depth_format(renderer);
  renderer->depth_format = depth_format;
  create_image(renderer, renderer->swap_extent.width,
               renderer->swap_extent.height, depth_format,
               VK_IMAGE_TILING_OPTIMAL,
               VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &renderer->depth_image,
               &renderer->depth_image_memory);

  create_image_views(renderer, renderer->depth_image, depth_format,
                     VK_IMAGE_ASPECT_DEPTH_BIT, &renderer->depth_image_view);
}
