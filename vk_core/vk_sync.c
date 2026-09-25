//
// Created by saad on 9/25/26.
//
#include "./include/vk_sync.h"

VkResult create_sync_object(renderer *renderer) {
    VkSemaphoreCreateInfo prsentation_semaphore_info = {0};
    prsentation_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkSemaphoreCreateInfo render_finished_semaphore_info = {0};
    render_finished_semaphore_info.sType =
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult res =
        vkCreateSemaphore(renderer->my_device, &prsentation_semaphore_info, NULL,
                          &renderer->present_complete_semaphore);
    if (res != VK_SUCCESS) {
        return res;
    }
    res = vkCreateSemaphore(renderer->my_device, &render_finished_semaphore_info,
                            NULL, &renderer->render_finisheds_semaphor);

    if (res != VK_SUCCESS) {
        return res;
    }
    VkFenceCreateInfo draw_fence_info = {0};
    draw_fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    draw_fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    res = vkCreateFence(renderer->my_device, &draw_fence_info, NULL,
                        &renderer->draw_fence);

    if (res != VK_SUCCESS) {
        return res;
    }

    return VK_SUCCESS;
}