//
// Created by saad on 9/6/26.
//
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include "constant.h"
#include "init.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

void create_window(renderer *renderer) {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to init GLFW");
    return;
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  renderer->window =
      glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Renderer", NULL, NULL);
  if (!renderer->window) {
    fprintf(stderr, "Failed to Create GLFW window \n");
    glfwTerminate();

    return;
  }
}
VkResult draw_frame(renderer *renderer) {

  VkResult res = vkWaitForFences(renderer->my_device, 1, &renderer->draw_fence,
                                 VK_TRUE, UINT64_MAX);

  if (res != VK_SUCCESS) {
    return res;
  }

  res = vkResetFences(renderer->my_device, 1, &renderer->draw_fence);

  if (res != VK_SUCCESS) {
    return res;
  }

  uint32_t image_index;
  res = vkAcquireNextImageKHR(renderer->my_device, renderer->my_swapchain,
                              UINT64_MAX, renderer->present_complete_semaphore,
                              NULL, &image_index);

  if (res != VK_SUCCESS) {
    return res;
  }

  record_cmd_buffer(renderer, image_index);
  VkPipelineStageFlags wait_dst_stg_msk =
      VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo submitinfo = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &renderer->present_complete_semaphore,
      .pWaitDstStageMask = &wait_dst_stg_msk,
      .commandBufferCount = 1,
      .pCommandBuffers = &renderer->cmd_buff,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &renderer->render_finisheds_semaphor,
  };

  res = vkQueueSubmit(renderer->my_queue, 1, &submitinfo, renderer->draw_fence);

  if (res != VK_SUCCESS) {
    return res;
  }

  VkPresentInfoKHR present_info = {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &renderer->render_finisheds_semaphor,
      .swapchainCount = 1,
      .pSwapchains = &renderer->my_swapchain,
      .pImageIndices = &image_index,
  };

  res = vkQueuePresentKHR(renderer->my_queue, &present_info);
  if (res != VK_SUCCESS) {
    return res;
  }

  return VK_SUCCESS;
}

void main_loop(GLFWwindow *window, renderer *renderer) {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    draw_frame(renderer);
  }


}

void clean_up(GLFWwindow * window) {

  glfwDestroyWindow(window);
  glfwTerminate();
}