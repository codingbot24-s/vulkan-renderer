#include "include/vk_commandpool.h"
#include "../log/loger.h"

//
// Created by saad on 9/24/26.
//
void create_command_buffer(renderer *renderer) {

    VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = renderer->command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    const VkResult res = vkAllocateCommandBuffers(renderer->my_device, &command_buffer_allocate_info,&renderer->cmd_buff);
    if (res != VK_SUCCESS) {
        R_FATAL("cant allocate command buffer");
    }
}