//
// Created by saad on 9/24/26.
//

#include "./include/vk_graphics.h"
#include "../input.h"
#include "../log/loger.h"
VkResult create_shader_module(renderer* renderer, size_t code_size, const uint32_t* code, VkShaderModule* shader_module) {
    VkShaderModuleCreateInfo shader_module_create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = code_size,
        .pCode = code,
    };
    VkResult res = vkCreateShaderModule(renderer->my_device, &shader_module_create_info, NULL, shader_module);
    if (res != VK_SUCCESS) {
        return res;
    }


    return VK_SUCCESS;
}


void create_graphics_pipeline(renderer* renderer) {
    const char *file_path = "/home/saad/code/c/vulkan-renderer/shader/slang.spv";
    uint32_t *code_buffer = NULL;
    size_t code_size;
    read_file(file_path, &code_buffer, &code_size);

    VkShaderModule shader_module;
    if (create_shader_module(renderer, code_size, code_buffer, &shader_module) !=
        VK_SUCCESS) {
        free(code_buffer);
        return;
        }
    VkPipelineRenderingCreateInfo pipeline_rendering_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = NULL,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &renderer->surface_format.format,
    };

    VkPipelineShaderStageCreateInfo vertex_shader_stage = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = shader_module,
        .pName = "vertMain",
    };

    VkPipelineShaderStageCreateInfo fragment_shader_stage = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = shader_module,
        .pName = "fragMain",
    };

    VkPipelineShaderStageCreateInfo shader_stages[] = {
        vertex_shader_stage,
        fragment_shader_stage,
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };

    VkPipelineViewportStateCreateInfo viewportstate_create_info = {
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
        R_FATAL("Failed to create pipeline layout");
    }
    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
    };

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &pipeline_rendering_create_info,
        .stageCount = 2,
        .pStages = shader_stages,
        .pVertexInputState = &vertex_input_state_create_info,
        .pInputAssemblyState = &input_assembly_state_create_info,
        .pViewportState = &viewportstate_create_info,
        /// we were missing this structer thats why segfault
      .pMultisampleState = &multisampling,
      .pRasterizationState = &rasterizer,
      .pColorBlendState = &color_blending,
      .pDynamicState = &dynamic_state_create_info,
      .layout = renderer->pipeline_layout,
      .renderPass = NULL,
    };

    res = vkCreateGraphicsPipelines(renderer->my_device,NULL,1,&graphics_pipeline_create_info,NULL,&renderer->graphics_pipeline);
    if (res != VK_SUCCESS) {
        R_FATAL("Failed to create graphics pipeline");
    }

}