//
// Created by saad on 10/7/26.
//

#ifndef VULKAN_RENDERER_VK_TEXTURE_H
#define VULKAN_RENDERER_VK_TEXTURE_H

#include "../../renderer.h"

void create_texture_image(renderer *renderer);
void create_texture_image_view(renderer *renderer);
void create_texture_sampler(renderer *renderer);
#endif // VULKAN_RENDERER_VK_TEXTURE_H
