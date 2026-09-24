//
// Created by saad on 9/24/26.
//

#ifndef VULKAN_RENDERER_VK_INSTANCE_H
#define VULKAN_RENDERER_VK_INSTANCE_H

#include <vulkan/vulkan_core.h>

VkInstance create_instance();
void setup_debug_messenger(VkInstance);
#endif // VULKAN_RENDERER_VK_INSTANCE_H
