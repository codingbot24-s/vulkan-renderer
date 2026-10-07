//
// Created by saad on 9/24/26.
//

#ifndef VULKAN_RENDERER_RENDERER_H
#define VULKAN_RENDERER_RENDERER_H

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
  VkCommandPool command_pool;
  uint32_t graphics_queue_index;
  VkCommandBuffer *command_buffers;
  VkPipelineLayout pipeline_layout;
  VkPipeline graphics_pipeline;
  VkDeviceMemory vertex_buffer_memory;
  VkBuffer vertex_buffer;
  VkBuffer index_buffer;
  VkDeviceMemory index_buffer_memory;

  VkDescriptorSetLayout descriptor_layout;
  VkBuffer *uniform_buffers;
  VkDeviceMemory *uniform_buffer_memory;
  void **uniform_buffer_mapped;

  VkDescriptorPool descriptor_pool;
  VkDescriptorSet *descriptor_sets;
  /// sync objects
  uint32_t frame_index;
  uint32_t image_index;
  VkSemaphore *image_available_semaphore;
  VkSemaphore *render_finished_semaphore;
  VkFence *draw_fence;

  VkImage depth_image;
  VkDeviceMemory depth_image_memory;
  VkImageView depth_image_view;
  VkFormat depth_format;

  // staging buffer for texture
  VkBuffer staging_buffer;
  VkDeviceMemory stagin_buffer_memory;
  VkImage texture_image;
  VkDeviceMemory texture_image_memory;
} renderer;

#endif // VULKAN_RENDERER_RENDERER_H
