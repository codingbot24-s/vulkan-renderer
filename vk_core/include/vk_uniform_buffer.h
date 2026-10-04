//
// Created by saad on 10/2/26.
//

#ifndef VULKAN_RENDERER_VK_UNIFORM_BUFFER_H
#define VULKAN_RENDERER_VK_UNIFORM_BUFFER_H
#include "../../renderer.h"
void create_uniform_buffer(renderer *renderer);
void update_uniform_buffer(uint32_t current_image, renderer *renderer);
#endif //VULKAN_RENDERER_VK_UNIFORM_BUFFER_H
