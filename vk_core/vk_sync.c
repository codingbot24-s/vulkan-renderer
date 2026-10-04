//
// Created by saad on 9/25/26.
//
#include "./include/vk_sync.h"
#include "../constant.h"
#include "../log/loger.h"
#include "vulkan/vulkan_core.h"
#include <stdlib.h>


VkResult create_sync_object(renderer *renderer) {
  VkResult res;
  renderer->image_available_semaphore = malloc(sizeof(VkSemaphore) * 2);

  renderer->render_finished_semaphore =
      malloc(sizeof(VkSemaphore) * renderer->swapchain_image_count);

  renderer->draw_fence = malloc(sizeof(VkFence) * 2);

  //// NOTE: Image count here and image size is same ?
  /// this structer will work for both semaphore ?
  VkSemaphoreCreateInfo semaphore_create_info = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = NULL,
  };
  for (int i = 0; i < renderer->swapchain_image_count; ++i) {

    if (vkCreateSemaphore(renderer->my_device, &semaphore_create_info, NULL,
                            &renderer->render_finished_semaphore[i]) != VK_SUCCESS) {
      R_FATAL("error creating render_finished_semaphore ");
    }


  }

  VkFenceCreateInfo fence_create_info = {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
      .pNext = NULL,
      .flags = VK_FENCE_CREATE_SIGNALED_BIT,
  };
  for (int i = 0; i < MAX_FRAME_IN_FLIGHT; ++i) {
    res = vkCreateSemaphore(renderer->my_device, &semaphore_create_info, NULL,
                            &renderer->image_available_semaphore[i]);
    if (res != VK_SUCCESS) {
      R_FATAL("error creating image available semaphore ");
    }
    res = vkCreateFence(renderer->my_device, &fence_create_info, NULL,
                        &renderer->draw_fence[i]);
    if (res != VK_SUCCESS) {
      R_FATAL("error creating fences");
    }
  }

  return res;
}
