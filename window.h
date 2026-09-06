//
// Created by saad on 9/6/26.
//

#ifndef VULKAN_RENDERER_WINDOW_H
#define VULKAN_RENDERER_WINDOW_H
#include <GLFW/glfw3.h>

GLFWwindow *create_window();
void main_loop(GLFWwindow *window);
void clean_up(GLFWwindow *window);
#endif // VULKAN_RENDERER_WINDOW_H
