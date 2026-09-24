//
// Created by saad on 9/6/26.
//

#ifndef VULKAN_RENDERER_INIT_H
#define VULKAN_RENDERER_INIT_H
#include "renderer.h"

void run_app();
VkResult record_cmd_buffer(renderer *renderer, uint32_t image_index);
#endif // VULKAN_RENDERER_INIT_H
