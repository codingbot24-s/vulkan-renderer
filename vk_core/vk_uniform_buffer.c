//
// Created by saad on 10/2/26.
//
#include "./include/vk_uniform_buffer.h"
#include "../constant.h"
#include "./include/vk_buffer.h"

#include "../libs/cglm/include/cglm/affine.h"
#include "../libs/cglm/include/cglm/cam.h"
#include "../log/loger.h"
#include "cglm/affine-pre.h"
#include "cglm/util.h"
#include "vulkan/vulkan_core.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

typedef struct uniform_buffer_obj {
  mat4 model;
  mat4 view;
  mat4 proj;
} uniform_buffer_obj;

void create_uniform_buffer(renderer *renderer) {

  renderer->uniform_buffers = malloc(sizeof(VkBuffer) * MAX_FRAME_IN_FLIGHT);
  renderer->uniform_buffer_memory =
      malloc(sizeof(VkDeviceMemory) * MAX_FRAME_IN_FLIGHT);

  renderer->uniform_buffer_mapped =
      malloc(sizeof(void *) * MAX_FRAME_IN_FLIGHT);

  for (uint32_t i = 0; i < MAX_FRAME_IN_FLIGHT; ++i) {
    renderer->uniform_buffer_mapped[i] = NULL;
  }

  for (uint32_t i = 0; i < MAX_FRAME_IN_FLIGHT; ++i) {
    VkDeviceSize buffer_size = sizeof(uniform_buffer_obj);

    if (create_buffer(renderer, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_SHARING_MODE_EXCLUSIVE,
                      &renderer->uniform_buffers[i]) != VK_SUCCESS) {
      R_FATAL("error creating uniform buffers");
    }
    VkMemoryRequirements uniform_buffer_memory_req;
    vkGetBufferMemoryRequirements(renderer->my_device,
                                  renderer->uniform_buffers[i],
                                  &uniform_buffer_memory_req);
    if (allocate_buffer(
            renderer,
            find_memory_type(uniform_buffer_memory_req.memoryTypeBits,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             renderer),
            uniform_buffer_memory_req.size,
            &renderer->uniform_buffer_memory[i]) != VK_SUCCESS) {

      R_FATAL("error allocting for uniform buffer");
    }
    if (bind_buffer_memory(renderer, renderer->uniform_buffers[i],
                           renderer->uniform_buffer_memory[i],
                           0) != VK_SUCCESS) {
      R_FATAL("error binding for uniform buffer");
    }

    if (map_memory(renderer, renderer->uniform_buffer_memory[i], 0, buffer_size,
                   &renderer->uniform_buffer_mapped[i]) != VK_SUCCESS) {

      R_FATAL("error mapping in uniform buffer");
    }
  }
}

void update_uniform_buffer(uint32_t current_image, renderer *renderer) {
  uniform_buffer_obj ubo = {0};

  glm_mat4_identity(ubo.model);

  vec3 axis = {0.0f, 0.0f, 0.0f};
  glm_rotate(ubo.model, glm_rad(0), axis);

  vec3 up = {0.0f, 0.0f, 1.0f};
  glm_lookat(renderer->camera->pos, renderer->camera->target, up, ubo.view);

  float aspect_ratio =
      (float)renderer->swap_extent.width / renderer->swap_extent.height;
  glm_perspective(glm_rad(45.0f), aspect_ratio, 0.1f, 10.0f, ubo.proj);

  memcpy(renderer->uniform_buffer_mapped[current_image], &ubo, sizeof(ubo));
}
