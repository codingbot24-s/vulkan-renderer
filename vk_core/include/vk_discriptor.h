//
// Created by saad on 9/30/26.
//

#ifndef VULKAN_RENDERER_VK_DISCRIPTOR_H
#define VULKAN_RENDERER_VK_DISCRIPTOR_H
#include "../../renderer.h"

void create_descriptorset_layout(renderer *renderer);
void create_descriptor_set(renderer *renderer);
void create_descriptor_pool(renderer *renderer);
#endif // VULKAN_RENDERER_VK_DISCRIPTOR_H
