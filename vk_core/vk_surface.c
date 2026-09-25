//
// Created by saad on 9/24/26.
//

#define GLFW_INCLUDE_VULKAN
#include "../log/loger.h"
#include "../renderer.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

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

void create_surface(renderer *renderer) {

  VkSurfaceKHR surface = VK_NULL_HANDLE;

  VkResult res = glfwCreateWindowSurface(renderer->my_vk_instance,
                                         renderer->window, NULL, &surface);
  if (res != VK_SUCCESS || surface == NULL) {
    R_FATAL("error creating vulkan surface %d", res);
  }

  renderer->my_surface = surface;
}
