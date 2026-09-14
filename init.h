//
// Created by saad on 9/6/26.
//

#ifndef VULKAN_RENDERER_INIT_H
#define VULKAN_RENDERER_INIT_H

#include <GLFW/glfw3.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>
typedef struct renderer {
  GLFWwindow *window;
  VkInstance my_vk_instance;
  VkSurfaceKHR my_surface;
  VkPhysicalDevice my_physical_device;
  VkDevice my_device;
  VkQueue my_queue;
  VkSwapchainKHR my_swapchain;
  VkImage *swapchain_images;
  VkExtent2D swap_extent;
  VkSurfaceFormatKHR surface_format;
  VkImageView *swapchain_image_views;
  uint32_t swapchain_image_count;
} renderer;
void run_app();
#endif // VULKAN_RENDERER_INIT_H
