//
// Created by saad on 9/6/26.
//
#include <stdint.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include "./log/loger.h"
#include "./vk_core/include/vk_command_buffers.h"
#include "constant.h"
#include "renderer.h"
#include "vk_core/include/vk_uniform_buffer.h"
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
  VkResult res = vkWaitForFences(renderer->my_device, 1,
                                 &renderer->draw_fence[renderer->frame_index],
                                 VK_TRUE, UINT64_MAX);
  if (res != VK_SUCCESS) {
    R_FATAL("wait for fence failed");
  }

  res = vkResetFences(renderer->my_device, 1,
                      &renderer->draw_fence[renderer->frame_index]);
  if (res != VK_SUCCESS) {
    R_FATAL("wait for fence failed");
  }
  /// this call need the fence in unsignaled state
  // uint32_t image_index = 0;
  res = vkAcquireNextImageKHR(
      renderer->my_device, renderer->my_swapchain, UINT64_MAX,
      renderer->image_available_semaphore[renderer->frame_index], NULL,
      &renderer->image_index);
  /// what if we do this for testing
  if (res != VK_SUCCESS)  {
    // R_FATAL("cant get the image");
    printf("res != vk success res = %d  image index is %d", res, renderer->image_index);
  }

  update_uniform_buffer(renderer->frame_index, renderer);
  /// NOTE: i am still thinking about these do we need to reset the command
  /// buffer we
  ///  will try but for now our renderer is working here we
  // res =
  //     vkResetCommandBuffer(renderer->command_buffers[renderer->frame_index],
  //     0);
  //
  // if (res != VK_SUCCESS) {
  //   R_FATAL("error reseting the command buffer");
  // }

  res = record_cmd_buffer(renderer, renderer->image_index);
  if (res != VK_SUCCESS) {
    R_FATAL("error recording command buffer");
  }

  VkPipelineStageFlags wait_stage_flag =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  VkSubmitInfo submit_info = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = NULL,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores =
          &renderer->image_available_semaphore[renderer->frame_index],
      .pWaitDstStageMask = &wait_stage_flag,
      .commandBufferCount = 1,
      .pCommandBuffers = &renderer->command_buffers[renderer->frame_index],
      .signalSemaphoreCount = 1,
      .pSignalSemaphores =
          &renderer->render_finished_semaphore[renderer->image_index],
  };
  res = vkQueueSubmit(renderer->my_queue, 1, &submit_info,
                      renderer->draw_fence[renderer->frame_index]);
  if (res != VK_SUCCESS) {
    R_FATAL("error submitting to queue");
  }
  VkPresentInfoKHR present_info = {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = NULL,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores =
          &renderer->render_finished_semaphore[renderer->image_index],
      .swapchainCount = 1,
      .pSwapchains = &renderer->my_swapchain,
      .pImageIndices = &renderer->image_index,
  };
  res = vkQueuePresentKHR(renderer->my_queue, &present_info);
  
  if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
    R_FATAL("presentation error");
  }
  renderer->frame_index = (renderer->frame_index + 1) % MAX_FRAME_IN_FLIGHT;

  return VK_SUCCESS;
}

void main_loop(GLFWwindow *window, renderer *renderer) {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    draw_frame(renderer);
  }
}

void clean_up(GLFWwindow *window) {

  glfwDestroyWindow(window);
  glfwTerminate();
}
