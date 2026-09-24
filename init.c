//
// Created by saad on 9/6/26.
//

#include "init.h"
#include "input.h"
#include "renderer.h"
#include "vk_core/include/vk_device.h"
#include "vk_core/include/vk_instance.h"
#include "vk_core/include/vk_surface.h"
#include "window.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

/* Helper functions */
uint32_t clamp(uint32_t value, uint32_t low, uint32_t high) {
  const uint32_t t = value < low ? low : value;
  return t > high ? high : t;
}

VkPresentModeKHR check_for_default_mode(VkPresentModeKHR *available_modes,
                                        uint32_t presentation_mode_count) {
  for (uint32_t i = 0; i < presentation_mode_count; ++i) {
    if (available_modes[i] == VK_PRESENT_MODE_FIFO_KHR) {
      return VK_PRESENT_MODE_FIFO_KHR;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkPresentModeKHR choose_swap_present_mode(renderer *renderer) {

  VkResult res;

  uint32_t presentation_mode_count = 0;
  res = vkGetPhysicalDeviceSurfacePresentModesKHR(
      renderer->my_physical_device, renderer->my_surface,
      &presentation_mode_count, NULL);

  if (res != VK_SUCCESS) {
    fprintf(stderr, "cant get the presentation counts %u \n", res);
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkPresentModeKHR *presentation_modes =
      malloc(presentation_mode_count * sizeof(VkPresentModeKHR));
  if (!presentation_modes) {
    fprintf(stderr, "cant allocate for presentation mode \n");
    return VK_PRESENT_MODE_FIFO_KHR;
  }
  res = vkGetPhysicalDeviceSurfacePresentModesKHR(
      renderer->my_physical_device, renderer->my_surface,
      &presentation_mode_count, presentation_modes);

  if (res != VK_SUCCESS) {
    fprintf(stderr, "cant get the presentation modes %u \n", res);
    free(presentation_modes);
    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkPresentModeKHR default_mode =
      check_for_default_mode(presentation_modes, presentation_mode_count);
  VkPresentModeKHR chossen_presentation_mode = {0};
  for (int i = 0; i < presentation_mode_count; ++i) {
    if (presentation_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      chossen_presentation_mode = presentation_modes[i];
      break;
    }
  }

  if (chossen_presentation_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
    return chossen_presentation_mode;
  }

  return default_mode;
}

void setup_images(renderer *renderer) {
  uint32_t swapchain_image_count = 0;
  VkResult res;
  res = vkGetSwapchainImagesKHR(renderer->my_device, renderer->my_swapchain,
                                &swapchain_image_count, NULL);

  if (res != VK_SUCCESS) {
    fprintf(stderr, ":cant get the image count for swapchain %u \n", res);
    return;
  }
  VkImage *images = malloc(swapchain_image_count * sizeof(VkImage));
  if (!images) {
    fprintf(stderr, ":cant allocated for getting images \n");
    return;
  }
  res = vkGetSwapchainImagesKHR(renderer->my_device, renderer->my_swapchain,
                                &swapchain_image_count, images);

  if (res != VK_SUCCESS) {
    free(images);
    fprintf(stderr, ":cant get the images for swapchain %u \n", res);
    return;
  }

  renderer->swapchain_image_count = swapchain_image_count;
  renderer->swapchain_images = images;
}

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

void create_command_pool(renderer *renderer) {
  VkCommandPoolCreateInfo cmd_pool_create_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = renderer->graphics_queue_index,
  };

  VkResult res = vkCreateCommandPool(renderer->my_device, &cmd_pool_create_info,
                                     NULL, &renderer->command_pool);

  if (res != VK_SUCCESS) {
    fprintf(stderr, "Cant create the command pool");

    return;
  }
}

void create_command_buffer(renderer *renderer) {
  VkCommandBufferAllocateInfo cmd_buff_alloc_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = renderer->command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };

  VkResult res = vkAllocateCommandBuffers(
      renderer->my_device, &cmd_buff_alloc_info, &renderer->cmd_buff);

  if (res != VK_SUCCESS) {
    fprintf(stderr, "Cant allocate the command buffer \n");

    return;
  }
}

VkResult create_shader_module(uint32_t *code, renderer *renderer,
                              size_t code_size, VkShaderModule *shader_module) {

  VkShaderModuleCreateInfo shader_module_info = {
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pNext = NULL,
      .codeSize = code_size,
      .pCode = code,
  };

  VkResult res = vkCreateShaderModule(renderer->my_device, &shader_module_info,
                                      NULL, shader_module);

  if (res != VK_SUCCESS) {
    return res;
  }

  return VK_SUCCESS;
}
void create_graphics_pipeline(renderer *renderer) {
  const char *file_path = "/home/saad/code/c/vulkan-renderer/shader/slang.spv";
  uint32_t *code_buffer = NULL;
  size_t code_size;
  read_file(file_path, &code_buffer, &code_size);

  VkShaderModule shader_module;
  if (create_shader_module(code_buffer, renderer, code_size, &shader_module) !=
      VK_SUCCESS) {
    free(code_buffer);
    return;
  }
  VkPipelineShaderStageCreateInfo shader_stage_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = shader_module,
      .pName = "vertMain"};

  VkPipelineShaderStageCreateInfo frag_stage_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = shader_module,
      .pName = "fragMain",
  };

  VkPipelineShaderStageCreateInfo shaderStages[] = {shader_stage_info,
                                                    frag_stage_info};

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,

  };

  VkPipelineInputAssemblyStateCreateInfo input_assembly = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
  };
  VkPipelineViewportStateCreateInfo viewportstate = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,

      .scissorCount = 1};
  VkDynamicState dynamic_state[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                    VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      /// NOTE: for now we can hardcode this
      .dynamicStateCount = 2,
      .pDynamicStates = dynamic_state,
  };

  VkPipelineRasterizationStateCreateInfo rasterizer = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = VK_CULL_MODE_BACK_BIT,
      .depthBiasEnable = VK_FALSE,
      .frontFace = VK_FRONT_FACE_CLOCKWISE,
      .lineWidth = 1.0f,
  };

  VkPipelineColorBlendAttachmentState color_attachment_state = {
      .blendEnable = VK_FALSE,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };

  VkPipelineColorBlendStateCreateInfo color_blending = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_COPY,
      .attachmentCount = 1,
      .pAttachments = &color_attachment_state,
  };

  VkPipelineLayoutCreateInfo pipeline_layoutinfo = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 0,
      .pushConstantRangeCount = 0,
  };

  VkResult res =
      vkCreatePipelineLayout(renderer->my_device, &pipeline_layoutinfo, NULL,
                             &renderer->pipeline_layout);

  if (res != VK_SUCCESS) {
    fprintf(stderr, "cant create the pipline layout \n");
    return;
  }
  VkPipelineRenderingCreateInfo pipe_line_rendering_create_info = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
      .colorAttachmentCount = 1,
      .pColorAttachmentFormats = &renderer->surface_format.format,
  };
  VkPipelineMultisampleStateCreateInfo multisampling = {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
  };

  VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = &pipe_line_rendering_create_info,
      .stageCount = 2,
      .pStages = shaderStages,
      .pVertexInputState = &vertex_input_info,
      .pInputAssemblyState = &input_assembly,
      .pViewportState = &viewportstate,
      /// we were missing this structer thats why segfault
      .pMultisampleState = &multisampling,
      .pRasterizationState = &rasterizer,
      .pColorBlendState = &color_blending,
      .pDynamicState = &dynamic_state_create_info,
      .layout = renderer->pipeline_layout,
      .renderPass = NULL,
  };

  res = vkCreateGraphicsPipelines(renderer->my_device, NULL, 1,
                                  &graphics_pipeline_create_info, NULL,
                                  &renderer->graphics_pipeline);
  if (res != VK_SUCCESS) {
    fprintf(stderr, "cant create the graphics pipeline %u \n", res);
    return;
  }
}

void transition_image_layout(renderer *renderer, uint32_t image_index,
                             VkImageLayout old_layout, VkImageLayout new_layout,
                             VkAccessFlags2 src_access_mask,
                             VkAccessFlags2 dst_access_mask,
                             VkPipelineStageFlags2 src_stage_mask,
                             VkPipelineStageFlags2 dst_stage_mask) {
  VkImageMemoryBarrier2 barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
      .srcStageMask = src_stage_mask,
      .srcAccessMask = src_access_mask,
      .dstStageMask = dst_stage_mask,
      .dstAccessMask = dst_access_mask,
      .oldLayout = old_layout,
      .newLayout = new_layout,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = renderer->swapchain_images[image_index],
      .subresourceRange =
          {
              .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
              .baseMipLevel = 0,
              .levelCount = 1,
              .baseArrayLayer = 0,
              .layerCount = 1,
          }

  };

  VkDependencyInfo dependency_info = {
      .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
      .dependencyFlags = {},
      .imageMemoryBarrierCount = 1,
      .pImageMemoryBarriers = &barrier,
  };

  vkCmdPipelineBarrier2(renderer->cmd_buff, &dependency_info);
}

VkResult record_cmd_buffer(renderer *renderer, uint32_t image_index) {
  VkCommandBufferBeginInfo cmd_begin_info = {0};
  cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  VkResult res = vkBeginCommandBuffer(renderer->cmd_buff, &cmd_begin_info);

  if (res != VK_SUCCESS) {
    return res;
  }

  transition_image_layout(renderer, image_index, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0,
                          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
  VkClearColorValue clear_color_value;
  clear_color_value.float32[0] = 0.0f;
  clear_color_value.float32[1] = 0.0f;
  clear_color_value.float32[2] = 0.0f;
  clear_color_value.float32[3] = 1.0f;

  VkRenderingAttachmentInfo attachment_info = {
      .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
      .clearValue = clear_color_value,
      .imageView = renderer->swapchain_image_views[image_index],
      .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
  };

  VkRenderingInfo rendering_info = {
      .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
      .renderArea = {.offset =
                         {
                             0,
                             0,
                         },
                     .extent = renderer->swap_extent},
      .layerCount = 1,
      .colorAttachmentCount = 1,
      .pColorAttachments = &attachment_info,
  };

  vkCmdBeginRendering(renderer->cmd_buff, &rendering_info);

  vkCmdBindPipeline(renderer->cmd_buff, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    renderer->graphics_pipeline);

  VkViewport viewport = {0};
  viewport.height = renderer->swap_extent.height;
  viewport.width = renderer->swap_extent.width;
  viewport.x = 0;
  viewport.y = 0;
  viewport.minDepth = 0;
  viewport.maxDepth = 1;
  vkCmdSetViewport(renderer->cmd_buff, 0, 1, &viewport);
  VkRect2D scissor = {0};
  scissor.extent.width = renderer->swap_extent.width;
  scissor.extent.height = renderer->swap_extent.height;

  vkCmdSetScissor(renderer->cmd_buff, 0, 1, &scissor);

  vkCmdDraw(renderer->cmd_buff, 3, 1, 0, 0);

  vkCmdEndRendering(renderer->cmd_buff);

  transition_image_layout(
      renderer, image_index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0,
      VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
      VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);

  res = vkEndCommandBuffer(renderer->cmd_buff);

  if (res != VK_SUCCESS) {
    return res;
  }

  return VK_SUCCESS;
}

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
  /// check for returned value
  create_sync_object(renderer);
}

void run_app() {
  renderer renderer;
  create_window(&renderer);

  init_vulkan(&renderer);
  main_loop(renderer.window, &renderer);
  clean_up(renderer.window);
}
