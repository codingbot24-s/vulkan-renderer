//
// Created by saad on 9/6/26.
//
#define GLFW_INCLUDE_VULKAN
#include "constant.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

int open_window() {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to init GLFW");
    return -1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Renderer", NULL, NULL);
  if (!window) {
    fprintf(stderr, "Failed to Create GLFW window");
    glfwTerminate();
    return -1;
  }

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}
