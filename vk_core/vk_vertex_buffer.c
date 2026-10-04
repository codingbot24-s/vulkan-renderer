//
// Created by saad on 10/2/26.
//

#include "./include/vk_vertex_buffer.h"
#include "../renderer.h"
#include "include/vk_buffer.h"
#include "../log/loger.h"

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
}

void create_vertex_buffer(renderer *renderer) {
    const vertex vertices[sizeof(vertex) * 3] = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

    VkVertexInputAttributeDescription input_attribute_descriptions[2];
    get_attribute_description(input_attribute_descriptions);

    VkResult res = create_buffer(
        renderer, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_SHARING_MODE_EXCLUSIVE, &renderer->vertex_buffer);
    if (res != VK_SUCCESS) {
        R_FATAL("cant create the vertex buffer");
    }

    VkMemoryRequirements buffer_memory_requirments;
    vkGetBufferMemoryRequirements(renderer->my_device, renderer->vertex_buffer,
                                  &buffer_memory_requirments);

    allocate_buffer(renderer,
                    find_memory_type(buffer_memory_requirments.memoryTypeBits,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                     renderer),
                    buffer_memory_requirments.size,
                    &renderer->vertex_buffer_memory);
    if (res != VK_SUCCESS) {
        R_FATAL("cant allocate the vertex buffer memory");
    }

    bind_buffer_memory(renderer, renderer->vertex_buffer,
                       renderer->vertex_buffer_memory, 0);
    if (res != VK_SUCCESS) {
        R_FATAL("cant bind the buffer memory");
    }

    void *data;
    map_memory(renderer, renderer->vertex_buffer_memory, 0, sizeof(vertices),
               &data);
    if (res != VK_SUCCESS) {
        R_FATAL("cant mapping memory to host");
    }
    r_copy(data, vertices, sizeof(vertices));
    vkUnmapMemory(renderer->my_device, renderer->vertex_buffer_memory);

#ifndef NDEBUG
    R_INFO("vertex buffer created");
#endif
}