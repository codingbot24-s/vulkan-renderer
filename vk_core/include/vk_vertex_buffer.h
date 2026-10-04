//
// Created by saad on 10/2/26.
//

#ifndef VULKAN_RENDERER_VK_VERTEX_BUFFER_H
#define VULKAN_RENDERER_VK_VERTEX_BUFFER_H
#include <vulkan/vulkan_core.h>
typedef struct vertex {
    float pos[2];
    float color[3];
} vertex;
void get_attribute_description(
    VkVertexInputAttributeDescription *attribute_descriptions);
VkVertexInputBindingDescription get_binding_description();
#endif //VULKAN_RENDERER_VK_VERTEX_BUFFER_H
