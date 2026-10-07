//
// Created by saad on 10/7/26.
//

#include "include/vk_buffer.h"
#include "include/vk_image.h"
#include "vulkan/vulkan_core.h"
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image.h"
#include "../log/loger.h"

void create_texture_image(renderer *renderer) {
  int texture_width, texture_height, texture_channels;

  stbi_uc *pixels =
      stbi_load("../texture/texture.jpg", &texture_width, &texture_height,
                &texture_channels, STBI_rgb_alpha);

  if (!pixels) {
    R_FATAL("error loading texture \n");
  }

  const VkDeviceSize image_size = texture_width * texture_width * 4;

  VkResult res =
      create_buffer(renderer, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_SHARING_MODE_EXCLUSIVE, &renderer->staging_buffer);

  if (res != VK_SUCCESS) {
    R_FATAL("error creating buffer for texture");
  }

  VkMemoryRequirements texture_buffer_memory_req = {0};

  vkGetBufferMemoryRequirements(renderer->my_device, renderer->staging_buffer,
                                &texture_buffer_memory_req);
  res = allocate_buffer(
      renderer,
      find_memory_type(texture_buffer_memory_req.memoryTypeBits,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       renderer),
      texture_buffer_memory_req.size, &renderer->stagin_buffer_memory);

  if (res != VK_SUCCESS) {
    R_FATAL("error allocating the memort for staging buffer \n");
  }

  res = bind_buffer_memory(renderer, renderer->staging_buffer,
                           renderer->stagin_buffer_memory, 0);

  if (res != VK_SUCCESS) {
    R_FATAL("error binding buffer memory");
  }

  void *data;
  res = map_memory(renderer, renderer->stagin_buffer_memory, 0, image_size,
                   &data);

  if (res != VK_SUCCESS) {
    R_FATAL("error mapping the texture buffer memory");
  }

  memcpy(data, pixels, image_size);

  vkUnmapMemory(renderer->my_device, renderer->stagin_buffer_memory);

  stbi_image_free(pixels);

  create_image(renderer, texture_width, texture_height, VK_FORMAT_R8G8B8A8_SRGB,
               VK_IMAGE_TILING_OPTIMAL,
               VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &renderer->texture_image,
               &renderer->texture_image_memory);

  /// ok so we will continue from here in the next stream
#ifndef NDEBUG
  R_INFO("texture created");
#endif
}
