//
// Created by saad on 10/2/26.
//

#ifndef VULKAN_RENDERER_VK_VERTEX_BUFFER_H
#define VULKAN_RENDERER_VK_VERTEX_BUFFER_H
#include "../../renderer.h"
#include <vulkan/vulkan_core.h>


void get_attribute_description(
    VkVertexInputAttributeDescription *attribute_descriptions);
VkVertexInputBindingDescription get_binding_description();

void create_vertex_buffer(renderer *renderer);

#endif // VULKAN_RENDERER_VK_VERTEX_BUFFER_H
