//
// Created by saad on 9/24/26.
//

#define GLFW_INCLUDE_VULKAN
#include "../log/loger.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "../renderer.h"
void create_surface(renderer *renderer) {

  VkSurfaceKHR surface = VK_NULL_HANDLE;

  VkResult res = glfwCreateWindowSurface(renderer->my_vk_instance,
                                         renderer->window, NULL, &surface);
  if (res != VK_SUCCESS || surface == NULL) {
    R_FATAL("error creating vulkan surface %d", res);
  }

  renderer->my_surface = surface;
}
