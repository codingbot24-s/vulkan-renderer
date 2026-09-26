//
// Created by saad on 9/26/26.
//

#include "include/vk_vertex_buffer.h"
#include <vulkan/vulkan.h>

typedef struct vertex {
  float pos[2];
  float color[3];
} vertex;

void create_vertex_buffer() {
  vertex vertices[sizeof(vertex) * 3] = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                         {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                         {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
  VkBufferCreateInfo buffer_create_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .size = sizeof(vertices),
  };
}
