//
// Created by saad on 10/7/26.
//

#include "include/vk_buffer.h"
#include "include/vk_image.h"
#include "vulkan/vulkan_core.h"
#include <stdint.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image.h"
#include "../log/loger.h"
#include "./include/vk_command_buffers.h"

#define IMAGE_PATH "/home/saad/Downloads/obj/iscv2_u1_v2.jpg"


void create_texture_image(renderer *renderer) {
  int texture_width, texture_height, texture_channels;

  stbi_uc *pixels =
      stbi_load(IMAGE_PATH, &texture_width,
                &texture_height, &texture_channels, STBI_rgb_alpha);

  if (!pixels) {
    R_FATAL("error loading texture \n");
  }

  const VkDeviceSize image_size = texture_width * texture_height * 4;

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

  begin_one_time_command_buffer(renderer);
  transition_image_layout(renderer, renderer->texture_image,
                          renderer->texture_image_transformation_command_buffer,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, 0, 0, 0,
                          VK_IMAGE_ASPECT_COLOR_BIT);

  copy_buffer_to_image(renderer->texture_image_transformation_command_buffer,
                       &renderer->staging_buffer, &renderer->texture_image,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       (uint32_t)texture_width, (uint32_t)texture_height);
  transition_image_layout(renderer, renderer->texture_image,
                          renderer->texture_image_transformation_command_buffer,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 0, 0, 0,
                          VK_IMAGE_ASPECT_COLOR_BIT);
  end_one_time_command_buffer(renderer);
}

void create_texture_image_view(renderer *renderer) {

  create_image_views(renderer, renderer->texture_image, VK_FORMAT_R8G8B8A8_SRGB,
                     VK_IMAGE_ASPECT_COLOR_BIT, &renderer->texture_image_view);
}
void create_texture_sampler(renderer *renderer) {

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(renderer->my_physical_device, &properties);
  VkSamplerCreateInfo sampler_info = {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .magFilter = VK_FILTER_LINEAR,
      .minFilter = VK_FILTER_LINEAR,
      .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
      .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .anisotropyEnable = VK_TRUE,
      .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
      .compareEnable = VK_TRUE,
      .compareOp = VK_COMPARE_OP_ALWAYS,
  };

  VkResult res = vkCreateSampler(renderer->my_device, &sampler_info, NULL,
                                 &renderer->sampler);

  if (res != VK_SUCCESS) {
    R_FATAL("error creating sampler");
  }
}
