//
// Created by saad on 9/6/26.
//

#include "init.h"
#include "./load_model.h"
#include "renderer.h"
#include "vk_core/include/vk_command_buffers.h"
#include "vk_core/include/vk_commandpool.h"
#include "vk_core/include/vk_device.h"
#include "vk_core/include/vk_discriptor.h"
#include "vk_core/include/vk_graphics.h"
#include "vk_core/include/vk_index_buffer.h"
#include "vk_core/include/vk_instance.h"
#include "vk_core/include/vk_surface.h"
#include "vk_core/include/vk_swapchain.h"
#include "vk_core/include/vk_sync.h"
#include "vk_core/include/vk_texture.h"
#include "vk_core/include/vk_uniform_buffer.h"
#include "vk_core/include/vk_vertex_buffer.h"
#include "vk_core/include/vk_zbuffer.h"
#include "window.h"

#define PATH_NAME "/home/saad/code/c/vulkan-renderer/models/viking_room.obj"

void init_vulkan(renderer *renderer) {
  renderer->my_vk_instance = create_instance();
  setup_debug_messenger(renderer->my_vk_instance);
  create_surface(renderer);
  pick_physical_device(renderer);
  create_logical_device(renderer);
  create_swapchain(renderer);
  create_swapchain_image_view(renderer);
  create_command_pool(renderer);
  create_command_buffer(renderer);
  create_depth_resource(renderer);
  create_texture_image(renderer);
  create_texture_image_view(renderer);
  create_texture_sampler(renderer);
  /// TODO: remove this function from here it is not vulkan function
  load_model(PATH_NAME, renderer);
  create_vertex_buffer(renderer);
  create_index_buffer(renderer);
  create_uniform_buffer(renderer);
  create_descriptor_pool(renderer);
  create_descriptorset_layout(renderer);
  create_descriptor_set(renderer);
  create_graphics_pipeline(renderer);
  create_sync_object(renderer);
}

void run_app() {
  renderer renderer;
  renderer.frame_index = 0;
  renderer.image_index = 0;
  create_window(&renderer);

  init_vulkan(&renderer);
  main_loop(renderer.window, &renderer);
  clean_up(renderer.window);
}
