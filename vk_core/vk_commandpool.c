//
// Created by saad on 9/24/26.
//
#include "./include/vk_commandpool.h"
#include "../renderer.h"
#include <stdio.h>

void create_command_pool(renderer *renderer) {
    VkCommandPoolCreateInfo cmd_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = renderer->graphics_queue_index,
    };

    VkResult res = vkCreateCommandPool(renderer->my_device, &cmd_pool_create_info,
                                       NULL, &renderer->command_pool);

    if (res != VK_SUCCESS) {
        fprintf(stderr, "Cant create the command pool");

        return;
    }
}