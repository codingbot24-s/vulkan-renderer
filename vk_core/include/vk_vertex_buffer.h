//
// Created by saad on 9/26/26.
//

#ifndef VULKAN_RENDERER_VK_VERTEX_BUFFER_H
#define VULKAN_RENDERER_VK_VERTEX_BUFFER_H

#include "../../renderer.h"
#include <vulkan/vulkan_core.h>

void create_vertex_buffer(renderer *renderer);
VkVertexInputBindingDescription get_binding_description();
void get_attribute_description(
    VkVertexInputAttributeDescription *attribute_descriptions);

void create_index_buffer(renderer *renderer);

#endif // VULKAN_RENDERER_VK_VERTEX_BUFFER_H
