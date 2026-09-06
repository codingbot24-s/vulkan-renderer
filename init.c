//
// Created by saad on 9/6/26.
//

#include "window.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>

void init_vulkan() {

  VkInstance instance;
  VkApplicationInfo app_info = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = NULL,
      .pApplicationName = "Renderer",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = NULL,
      .engineVersion = 0,
      .apiVersion = VK_API_VERSION_1_0,
  };

  uint32_t glfw_extension_count = 0;
  const char **glfw_extensions =
      glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  VkInstanceCreateInfo instance_creation_info = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = NULL,
      .pApplicationInfo = &app_info,
      .enabledExtensionCount = glfw_extension_count,
      .ppEnabledExtensionNames = glfw_extensions,
      .enabledLayerCount = 0,
  };

  if (vkCreateInstance(&instance_creation_info, NULL, &instance) !=
      VK_SUCCESS) {
    fprintf(stderr, "Cant create the vulkan instance \n");
    exit(EXIT_FAILURE);
  }

  printf("instance created  \n");
}

void run_app() {

  GLFWwindow *win = create_window();
  if (win == NULL) {
    exit(EXIT_FAILURE);
  }
  init_vulkan();
  main_loop(win);
  clean_up(win);
}
