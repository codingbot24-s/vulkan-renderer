//
// Created by saad on 9/6/26.
//

#ifndef VULKAN_RENDERER_WINDOW_H
#define VULKAN_RENDERER_WINDOW_H
#include "init.h"
#include <GLFW/glfw3.h>

void create_window(renderer *renderer);
void main_loop(GLFWwindow *window);
void clean_up(GLFWwindow *window);
#endif // VULKAN_RENDERER_WINDOW_H
