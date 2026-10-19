//
// Created by saad on 15/09/26.
//

#ifndef VULKAN_RENDERER_INPUT_H
#define VULKAN_RENDERER_INPUT_H

#include "renderer.h"
#include <stdint.h>

void handle_input(int key, int action, renderer *renderer);
char *read_file(const char *path, uint32_t **out_buffer, size_t *buffer_size);
#endif // VULKAN_RENDERER_INPUT_H
