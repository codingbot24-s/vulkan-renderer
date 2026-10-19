//
// Created by saad on 9/24/26.
//

#include "../log/loger.h"
#include "../renderer.h"
#include "./include/vk_image.h"
#include "vulkan/vulkan_core.h"
#include <stdlib.h>

/*
    NOTE: after changing the command buffer to * we have changed the command
   buffer access
*/

VkResult record_cmd_buffer(renderer *renderer, uint32_t image_index) {

  VkCommandBufferBeginInfo cmd_begin_info = {0};
  cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  VkResult res = vkBeginCommandBuffer(
      renderer->command_buffers[renderer->frame_index], &cmd_begin_info);

  if (res != VK_SUCCESS) {
    return res;
  }
  VkImage swapchain_image = renderer->swapchain_images[image_index];
  transition_image_layout(renderer, swapchain_image,
                          &renderer->command_buffers[renderer->frame_index],
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0,
                          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VK_IMAGE_ASPECT_COLOR_BIT);
  VkClearColorValue clear_color_value;
  clear_color_value.float32[0] = 0.0f;
  clear_color_value.float32[1] = 0.0f;
  clear_color_value.float32[2] = 0.0f;
  clear_color_value.float32[3] = 1.0f;

  transition_image_layout(renderer, renderer->depth_image,
                          &renderer->command_buffers[renderer->frame_index],
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                          VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                              VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                          VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                              VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                          VK_IMAGE_ASPECT_DEPTH_BIT);

  VkClearDepthStencilValue clear_depth = {.depth = 1.0f};

  VkRenderingAttachmentInfo attachment_info = {
      .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
      .clearValue = clear_color_value,
      .imageView = renderer->swapchain_image_views[image_index],
      .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
  };

  VkRenderingAttachmentInfo depth_attachment_info = {
      .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
      .clearValue = clear_depth.depth,
      .imageView = renderer->depth_image_view,
      .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
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
      .pDepthAttachment = &depth_attachment_info};

  vkCmdBeginRendering(renderer->command_buffers[renderer->frame_index],
                      &rendering_info);

  vkCmdBindPipeline(renderer->command_buffers[renderer->frame_index],
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    renderer->graphics_pipeline);

  VkDeviceSize offsets[1] = {0};
  vkCmdBindVertexBuffers(renderer->command_buffers[renderer->frame_index], 0, 1,
                         &renderer->vertex_buffer, offsets);

  // vkCmdBindIndexBuffer(renderer->command_buffers[renderer->frame_index],
  //                      renderer->index_buffer, 0, VK_INDEX_TYPE_UINT32);

  VkViewport viewport = {0};
  viewport.height = -(float)renderer->swap_extent.height;
  viewport.width = (float)renderer->swap_extent.width;
  viewport.x = 0;
  viewport.y = renderer->swap_extent.height;
  viewport.minDepth = 0;
  viewport.maxDepth = 1;
  vkCmdSetViewport(renderer->command_buffers[renderer->frame_index], 0, 1,
                   &viewport);
  VkRect2D scissor = {0};
  scissor.extent.width = renderer->swap_extent.width;
  scissor.extent.height = renderer->swap_extent.height;
  vkCmdSetScissor(renderer->command_buffers[renderer->frame_index], 0, 1,
                  &scissor);
  // so we are seeing this rectangle because this indices we need to make it
  // cube indices
  vkCmdBindDescriptorSets(
      renderer->command_buffers[renderer->frame_index],

      VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->pipeline_layout, 0, 1,
      &renderer->descriptor_sets[renderer->frame_index], 0, 0);
  /// NOTE: hardcoded 6 for rectangle change this yes we need to change this
  /// because our indices is changed 6 is wrong for cube we need the 36
  /// vkCmdDrawIndexed(renderer->command_buffers[renderer->frame_index], 6, 1,
  /// 0, 0,
  ///                0);
  //
  /// REMOVE: this draw call this is draw call without using index buffers
  vkCmdDraw(renderer->command_buffers[renderer->frame_index],
            renderer->vertices_count, 1, 0, 0);
  vkCmdEndRendering(renderer->command_buffers[renderer->frame_index]);

  transition_image_layout(
      renderer, swapchain_image,
      &renderer->command_buffers[renderer->frame_index],
      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0,
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
      VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

  res = vkEndCommandBuffer(renderer->command_buffers[renderer->frame_index]);

  if (res != VK_SUCCESS) {
    return res;
  }

  return VK_SUCCESS;
}

//// NOTE: we can use this for creating all the command buffers and remove some
/// repeated code
void create_command_buffer(renderer *renderer) {

  const VkCommandBufferAllocateInfo command_buffer_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = NULL,
      .commandPool = renderer->command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 2,
  };

  renderer->command_buffers = malloc(sizeof(VkCommandBuffer) * 2);
  const VkResult res = vkAllocateCommandBuffers(renderer->my_device,
                                                &command_buffer_allocate_info,
                                                renderer->command_buffers);

  if (res != VK_SUCCESS) {
    R_FATAL("cant allocate command buffer");
  }
}

/// allocate and begin one time command buffer
/// NOTE: this function shoudnt access the renderer it should only create and
/// return the command buffer add this
void begin_one_time_command_buffer(renderer *renderer) {

  const VkCommandBufferAllocateInfo command_buffer_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = NULL,
      .commandPool = renderer->command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };

  renderer->texture_image_transformation_command_buffer =
      malloc(sizeof(VkCommandBuffer) * 1);

  VkResult res = vkAllocateCommandBuffers(
      renderer->my_device, &command_buffer_allocate_info,
      renderer->texture_image_transformation_command_buffer);

  if (res != VK_SUCCESS) {
    R_FATAL("cant allocate command buffer");
  }

  VkCommandBufferBeginInfo command_buffer_begin_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = NULL,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
  };
  res = vkBeginCommandBuffer(
      *renderer->texture_image_transformation_command_buffer,
      &command_buffer_begin_info);

  if (res != VK_SUCCESS) {
    R_FATAL("error begining one time command \n");
  }
}

/// NOTE: this function shoudnt access the renderer it should only end the given
/// command buffer
void end_one_time_command_buffer(renderer *renderer) {

  VkResult res;

  res = vkEndCommandBuffer(
      *renderer->texture_image_transformation_command_buffer);
  if (res != VK_SUCCESS) {
    R_FATAL("error ending command buffer");
  }

  const VkSubmitInfo submit_info = {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = NULL,
      .commandBufferCount = 1,
      .pCommandBuffers = renderer->texture_image_transformation_command_buffer};

  res = vkQueueSubmit(renderer->my_queue, 1, &submit_info, NULL); 

  if (res != VK_SUCCESS) {
    R_FATAL("error submitting queue");
  }

  res = vkQueueWaitIdle(renderer->my_queue);

  if (res != VK_SUCCESS) {
    R_FATAL("error waiting for queuee");
  }
}
