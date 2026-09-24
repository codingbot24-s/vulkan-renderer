//
// Created by saad on 9/24/26.
//

#include "./include/vk_image.h"
#include <stdlib.h>
#include <stdio.h>

void create_image_views(renderer *renderer) {
    VkImageViewCreateInfo image_view_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = renderer->surface_format.format,
        .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .layerCount = 1,
                .levelCount = 1,
            },

    };
    if (!renderer->swapchain_images) {
        return;
    }
    renderer->swapchain_image_views =
        malloc(renderer->swapchain_image_count * sizeof(VkImageView));
    if (!renderer->swapchain_image_views) {
        return;
    }
    for (int i = 0; i < renderer->swapchain_image_count; ++i) {
        image_view_info.image = renderer->swapchain_images[i];
        vkCreateImageView(renderer->my_device, &image_view_info, NULL,
                          &renderer->swapchain_image_views[i]);
    }
}


