//
// Created by saad on 9/6/26.
//

#include "init.h"
#include "renderer.h"
#include "vk_core/include/vk_command_buffers.h"
#include "vk_core/include/vk_commandpool.h"
#include "vk_core/include/vk_device.h"
#include "vk_core/include/vk_graphics.h"
#include "vk_core/include/vk_image.h"
#include "vk_core/include/vk_instance.h"
#include "vk_core/include/vk_surface.h"
#include "vk_core/include/vk_swapchain.h"
#include "vk_core/include/vk_sync.h"
#include "window.h"

#include <stdint.h>


void init_vulkan(renderer *renderer) {
  renderer->my_vk_instance = create_instance();
  setup_debug_messenger(renderer->my_vk_instance);
  create_surface(renderer);
  pick_physical_device(renderer);
  create_logical_device(renderer);
  create_swapchain(renderer);
  create_image_views(renderer);
  create_command_pool(renderer);
  create_command_buffer(renderer);
  create_graphics_pipeline(renderer);
  create_sync_object(renderer);
}

void run_app() {
  renderer renderer;
  create_window(&renderer);

  init_vulkan(&renderer);
  main_loop(renderer.window, &renderer);
  clean_up(renderer.window);
}
