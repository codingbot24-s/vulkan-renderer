//
// Created by saad on 9/6/26.
//
#define GLFW_INCLUDE_VULKAN
#include "constant.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

GLFWwindow *create_window() {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to init GLFW");
    return NULL;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Renderer", NULL, NULL);
  if (!window) {
    fprintf(stderr, "Failed to Create GLFW window");
    glfwTerminate();

    return NULL;
  }

  return window;
}

void main_loop(GLFWwindow *window) {

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}

void clean_up(GLFWwindow *window) {

  glfwDestroyWindow(window);
  glfwTerminate();
}
