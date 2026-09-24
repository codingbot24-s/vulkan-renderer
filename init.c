//
// Created by saad on 9/6/26.
//

#include "init.h"
#include "input.h"
#include "renderer.h"
#include "vk_core/include/vk_device.h"
#include "vk_core/include/vk_instance.h"
#include "vk_core/include/vk_surface.h"
#include "vk_core/include/vk_swapchain.h"
#include "vk_core/include/vk_commandpool.h"
#include "vk_core/include/vk_image.h"

#include "window.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>


/* Helper functions */
uint32_t clamp(uint32_t value, uint32_t low, uint32_t high) {
  const uint32_t t = value < low ? low : value;
  return t > high ? high : t;
}

VkPresentModeKHR check_for_default_mode(VkPresentModeKHR *available_modes,
                                        uint32_t presentation_mode_count) {
  for (uint32_t i = 0; i < presentation_mode_count; ++i) {
    if (available_modes[i] == VK_PRESENT_MODE_FIFO_KHR) {
      return VK_PRESENT_MODE_FIFO_KHR;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkPresentModeKHR choose_swap_present_mode(renderer *renderer) {

  VkResult res;

  uint32_t presentation_mode_count = 0;
  res = vkGetPhysicalDeviceSurfacePresentModesKHR(
      renderer->my_physical_device, renderer->my_surface,
      &presentation_mode_count, NULL);

  if (res != VK_SUCCESS) {
    fprintf(stderr, "cant get the presentation counts %u \n", res);
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkPresentModeKHR *presentation_modes =
      malloc(presentation_mode_count * sizeof(VkPresentModeKHR));
  if (!presentation_modes) {
    fprintf(stderr, "cant allocate for presentation mode \n");
    return VK_PRESENT_MODE_FIFO_KHR;
  }
  res = vkGetPhysicalDeviceSurfacePresentModesKHR(
      renderer->my_physical_device, renderer->my_surface,
      &presentation_mode_count, presentation_modes);

  if (res != VK_SUCCESS) {
    fprintf(stderr, "cant get the presentation modes %u \n", res);
    free(presentation_modes);
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkPresentModeKHR default_mode =
      check_for_default_mode(presentation_modes, presentation_mode_count);
  VkPresentModeKHR chossen_presentation_mode = {0};
  for (int i = 0; i < presentation_mode_count; ++i) {
    if (presentation_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      chossen_presentation_mode = presentation_modes[i];
      break;
    }
  }

  if (chossen_presentation_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
    return chossen_presentation_mode;
  }

  return default_mode;
}

void setup_images(renderer *renderer) {
  uint32_t swapchain_image_count = 0;
  VkResult res;
  res = vkGetSwapchainImagesKHR(renderer->my_device, renderer->my_swapchain,
                                &swapchain_image_count, NULL);

  if (res != VK_SUCCESS) {
    fprintf(stderr, ":cant get the image count for swapchain %u \n", res);
    return;
  }
  VkImage *images = malloc(swapchain_image_count * sizeof(VkImage));
  if (!images) {
    fprintf(stderr, ":cant allocated for getting images \n");
    return;
  }
  res = vkGetSwapchainImagesKHR(renderer->my_device, renderer->my_swapchain,
                                &swapchain_image_count, images);

  if (res != VK_SUCCESS) {
    free(images);
    fprintf(stderr, ":cant get the images for swapchain %u \n", res);
    return;
  }

  renderer->swapchain_image_count = swapchain_image_count;
  renderer->swapchain_images = images;
}





void transition_image_layout(renderer *renderer, uint32_t image_index,
                             VkImageLayout old_layout, VkImageLayout new_layout,
                             VkAccessFlags2 src_access_mask,
                             VkAccessFlags2 dst_access_mask,
                             VkPipelineStageFlags2 src_stage_mask,
                             VkPipelineStageFlags2 dst_stage_mask) {
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
      .image = renderer->swapchain_images[image_index],
      .subresourceRange =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
          }

  };

  VkDependencyInfo dependency_info = {
      .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
      .dependencyFlags = {},
      .imageMemoryBarrierCount = 1,
      .pImageMemoryBarriers = &barrier,
  };

  vkCmdPipelineBarrier2(renderer->cmd_buff, &dependency_info);
}

VkResult record_cmd_buffer(renderer *renderer, uint32_t image_index) {
  VkCommandBufferBeginInfo cmd_begin_info = {0};
  cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  VkResult res = vkBeginCommandBuffer(renderer->cmd_buff, &cmd_begin_info);

  if (res != VK_SUCCESS) {
    return res;
  }

  transition_image_layout(renderer, image_index, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0,
                          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
  VkClearColorValue clear_color_value;
  clear_color_value.float32[0] = 0.0f;
  clear_color_value.float32[1] = 0.0f;
  clear_color_value.float32[2] = 0.0f;
  clear_color_value.float32[3] = 1.0f;

  VkRenderingAttachmentInfo attachment_info = {
      .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
      .clearValue = clear_color_value,
      .imageView = renderer->swapchain_image_views[image_index],
      .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
  };

  VkRenderingInfo rendering_info = {
      .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
      .renderArea = {.offset =
                         {
                             0,
                             0,
                         },
                     .extent = renderer->swap_extent},
      .layerCount = 1,
      .colorAttachmentCount = 1,
      .pColorAttachments = &attachment_info,
  };

  vkCmdBeginRendering(renderer->cmd_buff, &rendering_info);

  vkCmdBindPipeline(renderer->cmd_buff, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    renderer->graphics_pipeline);

  VkViewport viewport = {0};
  viewport.height = renderer->swap_extent.height;
  viewport.width = renderer->swap_extent.width;
  viewport.x = 0;
  viewport.y = 0;
  viewport.minDepth = 0;
  viewport.maxDepth = 1;
  vkCmdSetViewport(renderer->cmd_buff, 0, 1, &viewport);
  VkRect2D scissor = {0};
  scissor.extent.width = renderer->swap_extent.width;
  scissor.extent.height = renderer->swap_extent.height;

  vkCmdSetScissor(renderer->cmd_buff, 0, 1, &scissor);

  vkCmdDraw(renderer->cmd_buff, 3, 1, 0, 0);

  vkCmdEndRendering(renderer->cmd_buff);

  transition_image_layout(
      renderer, image_index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0,
      VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
      VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);

  res = vkEndCommandBuffer(renderer->cmd_buff);

  if (res != VK_SUCCESS) {
    return res;
  }

  return VK_SUCCESS;
}

VkResult create_sync_object(renderer *renderer) {
  VkSemaphoreCreateInfo prsentation_semaphore_info = {0};
  prsentation_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VkSemaphoreCreateInfo render_finished_semaphore_info = {0};
  render_finished_semaphore_info.sType =
      VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkResult res =
      vkCreateSemaphore(renderer->my_device, &prsentation_semaphore_info, NULL,
                        &renderer->present_complete_semaphore);
  if (res != VK_SUCCESS) {
    return res;
  }
  res = vkCreateSemaphore(renderer->my_device, &render_finished_semaphore_info,
                          NULL, &renderer->render_finisheds_semaphor);

  if (res != VK_SUCCESS) {
    return res;
  }
  VkFenceCreateInfo draw_fence_info = {0};
  draw_fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  draw_fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  res = vkCreateFence(renderer->my_device, &draw_fence_info, NULL,
                      &renderer->draw_fence);

  if (res != VK_SUCCESS) {
    return res;
  }

  return VK_SUCCESS;
}

void init_vulkan(renderer *renderer) {
  renderer->my_vk_instance = create_instance();
  setup_debug_messenger(renderer->my_vk_instance);
  create_surface(renderer);
  pick_physical_device(renderer);
  create_logical_device(renderer);
  create_swapchain(renderer);
  create_image_views(renderer);
  create_command_pool(renderer);
  create_command_buffer(renderer);
  create_graphics_pipeline(renderer);
  /// check for returned value
  create_sync_object(renderer);
}

void run_app() {
  renderer renderer;
  create_window(&renderer);

  init_vulkan(&renderer);
  main_loop(renderer.window, &renderer);
  clean_up(renderer.window);
}
