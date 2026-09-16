//
// Created by saad on 15/09/26.
//

#ifndef VULKAN_RENDERER_INPUT_H
#define VULKAN_RENDERER_INPUT_H

#include <stdint.h>
#include <stdlib.h>

char *read_file(const char *path, uint32_t **out_buffer, size_t *buffer_size);
#endif // VULKAN_RENDERER_INPUT_H
