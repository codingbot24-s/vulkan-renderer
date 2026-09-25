//
// Created by saad on 24/09/26.
//

#include "../renderer.h"
#include <stdbool.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>
uint32_t max(uint32_t a, uint32_t b) {
  if (a > b) {
    return a;
  }

  return b;
}
/* Helper functions */
uint32_t clamp(uint32_t value, uint32_t low, uint32_t high) {
  const uint32_t t = value < low ? low : value;
  return t > high ? high : t;
}

uint32_t choose_min_swap_image_count(VkSurfaceCapabilitiesKHR capabilities) {
  uint32_t min_image_count = max(3, capabilities.minImageCount);
  if ((0 < capabilities.maxImageCount) &&
      (capabilities.maxImageCount < min_image_count)) {
    min_image_count = capabilities.maxImageCount;
  }

  return min_image_count;
}

VkExtent2D choose_swap_extent_mode(VkSurfaceCapabilitiesKHR capabilities,
                                   renderer *renderer) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  }
  int width, height;
  glfwGetFramebufferSize(renderer->window, &width, &height);

  width = clamp(width, capabilities.minImageExtent.width,
                capabilities.maxImageExtent.width);
  height = clamp(height, capabilities.minImageExtent.height,
                 capabilities.maxImageExtent.height);

  VkExtent2D extent;
  extent.width = width;
  extent.height = height;

  return extent;
}

VkSurfaceFormatKHR choose_swap_surface_format(renderer *renderer) {
  VkResult res;
  VkSurfaceFormatKHR err_format = {VK_FORMAT_UNDEFINED,
                                   VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};

  uint32_t surface_format_count = 0;
  res = vkGetPhysicalDeviceSurfaceFormatsKHR(renderer->my_physical_device,
                                             renderer->my_surface,
                                             &surface_format_count, NULL);
  if (res != VK_SUCCESS) {
    fprintf(stderr, "cant get the surface count %u \n", res);
    return err_format;
  }

  VkSurfaceFormatKHR *surface_formats =
      malloc(surface_format_count * sizeof(VkSurfaceFormatKHR));
  if (!surface_formats) {
    fprintf(stderr, "cant allocate for surface formats \n");
    return err_format;
  }

  res = vkGetPhysicalDeviceSurfaceFormatsKHR(
      renderer->my_physical_device, renderer->my_surface, &surface_format_count,
      surface_formats);
  if (res != VK_SUCCESS) {
    fprintf(stderr, "cant get the surface formats %u \n", res);
    free(surface_formats);
    return err_format;
  }

  VkSurfaceFormatKHR choosen_surface_format = {0};
  for (int i = 0; i < surface_format_count; ++i) {
    if (surface_formats[i].format == VK_FORMAT_R8G8B8A8_SRGB &&
        surface_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      choosen_surface_format.format = surface_formats[i].format;
      choosen_surface_format.colorSpace = surface_formats[i].colorSpace;
    }
  }

  if (choosen_surface_format.format == 0 ||
      choosen_surface_format.colorSpace == 0) {
    choosen_surface_format.format = surface_formats[0].format;
    choosen_surface_format.colorSpace = surface_formats[0].colorSpace;
  }

  return choosen_surface_format;
}

void create_swapchain(renderer *renderer) {
  VkSurfaceCapabilitiesKHR capabilities = {0};
  VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      renderer->my_physical_device, renderer->my_surface, &capabilities);
  if (res != VK_SUCCESS) {
    fprintf(stderr, "cant get the surface capabilities %u \n", res);
    return;
  }
  /// Resolution of Images in swapchain
  VkExtent2D swap_extent = choose_swap_extent_mode(capabilities, renderer);
  renderer->swap_extent = swap_extent;

  uint32_t min_image_count = choose_min_swap_image_count(capabilities);
  /// choosen swap_chain_surface_format
  VkSurfaceFormatKHR choosen_surface_format =
      choose_swap_surface_format(renderer);
  renderer->surface_format = choosen_surface_format;

  VkPresentModeKHR choosen_presentation_mode =
      choose_swap_present_mode(renderer);

  VkSwapchainCreateInfoKHR swapchain_create_info = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = renderer->my_surface,
      .minImageCount = min_image_count,
      .imageFormat = choosen_surface_format.format,
      .imageColorSpace = choosen_surface_format.colorSpace,
      .imageExtent = swap_extent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .preTransform = capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = choosen_presentation_mode,
      .clipped = true,
      .oldSwapchain = NULL,
  };
  res = vkCreateSwapchainKHR(renderer->my_device, &swapchain_create_info, NULL,
                             &renderer->my_swapchain);

  if (res != VK_SUCCESS) {
    fprintf(stderr, "Cant create the swapchain %u \n", res);
    return;
  }

  setup_images(renderer);
}
