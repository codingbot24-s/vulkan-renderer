//
// Created by saad on 9/24/26.
//

#ifndef VULKAN_RENDERER_VK_COMMAND_BUFFERS_H
#define VULKAN_RENDERER_VK_COMMAND_BUFFERS_H
#include "../../renderer.h"
#include "vulkan/vulkan_core.h"
#include <stdint.h>

void create_command_buffer(renderer *renderer);
VkResult record_cmd_buffer(renderer *renderer, uint32_t image_index);

#endif // VULKAN_RENDERER_VK_COMMAND_BUFFERS_H
