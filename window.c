//
// Created by saad on 9/6/26.
//
#define GLFW_INCLUDE_VULKAN
#include "constant.h"
#include "init.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

void create_window(renderer *renderer) {
  if (!glfwInit()) {
    fprintf(stderr, "Failed to init GLFW");
    return;
  }
  // we are doing this before creating window
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

void main_loop(GLFWwindow *window) {

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}

void clean_up(GLFWwindow *window) {

  glfwDestroyWindow(window);
  glfwTerminate();
}
