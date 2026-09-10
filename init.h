//
// Created by saad on 9/6/26.
//

#ifndef VULKAN_RENDERER_INIT_H
#define VULKAN_RENDERER_INIT_H

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
typedef struct renderer {
  GLFWwindow *window;
  VkInstance my_vk_instance;
  VkSurfaceKHR my_surface;
} renderer;
void run_app();
#endif // VULKAN_RENDERER_INIT_H
