#include "../log/loger.h"
#include "include/vk_commandpool.h"

//
// Created by saad on 9/24/26.
//

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

void create_command_buffer(renderer *renderer) {

  VkCommandBufferAllocateInfo command_buffer_allocate_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = NULL,
      .commandPool = renderer->command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
  };
  const VkResult res = vkAllocateCommandBuffers(
      renderer->my_device, &command_buffer_allocate_info, &renderer->cmd_buff);
  if (res != VK_SUCCESS) {
    R_FATAL("cant allocate command buffer");
  }
}
