//
// Created by saad on 9/26/26.
//

#include "include/vk_vertex_buffer.h"
#include "../log/loger.h"
#include "../renderer.h"
#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>

typedef struct vertex {
  float pos[2];
  float color[3];

} vertex;

VkVertexInputBindingDescription get_binding_description() {
  return (VkVertexInputBindingDescription){.binding = 0,
                                           .stride = sizeof(vertex),
                                           .inputRate =
                                               VK_VERTEX_INPUT_RATE_VERTEX};
}

void get_attribute_description(
    VkVertexInputAttributeDescription *attribute_descriptions) {

  attribute_descriptions[0].location = 0;
  attribute_descriptions[0].binding = 0;
  attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_descriptions[0].offset = 0;

  attribute_descriptions[1].location = 1;
  attribute_descriptions[1].binding = 0;
  attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_descriptions[1].offset = sizeof(float) * 2;

  // we cant return this
}

void create_vertex_buffer(renderer *renderer) {
  vertex vertices[sizeof(vertex) * 3] = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                         {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                         {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

  VkVertexInputAttributeDescription input_attribute_descriptions[2];
  get_attribute_description(input_attribute_descriptions);

  VkBufferCreateInfo buffer_create_info = {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = NULL,
      .size = sizeof(vertices),
      .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };

  VkResult res = vkCreateBuffer(renderer->my_device, &buffer_create_info, NULL,
                                &renderer->vertex_buffer);

  if (res != VK_SUCCESS) {
    R_FATAL("cant create the vertex buffer");
  }
  /// we will continue from here in the next stream
}
