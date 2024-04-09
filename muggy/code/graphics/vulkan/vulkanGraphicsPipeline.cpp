//********************************************************************
//  File:    vulkanGraphicsPipeline.cpp
//  Date:    Thu, 28 Mar 2024: 17:14
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanGraphicsPipeline.h"
#include "vulkanShaders.h"

namespace muggy::graphics::vulkan::pipeline
{
    namespace 
    {
        // TODO(klek): these should likely be part of a class
        VkPipelineLayout pipelineLayout;
        VkPipeline       graphicsPipeline;
    } // namespace anonymous
    
    bool createGraphicsPipeline( VkDevice device, VkRenderPass renderPass )
    {
        VkShaderModule vertShaderModule { };
        VkShaderModule fragShaderModule { };
        // TODO(klek): THIS NEEDS TO BE FIXED

        shaders::createShaderModule( device, vertShaderModule, shaders::engine_shader::triangleShaderVS);
        shaders::createShaderModule( device, fragShaderModule, shaders::engine_shader::triangleShaderFS);

        // Setting up shaderstages
        VkPipelineShaderStageCreateInfo vertShaderStageInfo { };
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo { };
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[ ] = 
        { 
            vertShaderStageInfo, 
            fragShaderStageInfo 
        };

        // Setting up vertex input
        VkPipelineVertexInputStateCreateInfo vertexInputInfo { };
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;

        // Setting up assembly input
        VkPipelineInputAssemblyStateCreateInfo inputAssembly { };
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Setting up viewport stage
        VkPipelineViewportStateCreateInfo viewportState { };
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        // Setting up rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer { };
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        // Setting up multisampling
        VkPipelineMultisampleStateCreateInfo multiSampling { };
        multiSampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multiSampling.sampleShadingEnable = VK_FALSE;
        multiSampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // Setting up colorblend
        VkPipelineColorBlendAttachmentState colorBlendAttachment { };
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
                                              VK_COLOR_COMPONENT_G_BIT | 
                                              VK_COLOR_COMPONENT_B_BIT | 
                                              VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending { };
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[ 0 ] = 0.0f;
        colorBlending.blendConstants[ 1 ] = 0.0f;
        colorBlending.blendConstants[ 2 ] = 0.0f;
        colorBlending.blendConstants[ 3 ] = 0.0f;

        // Setting up dynamic states
        utils::vector<VkDynamicState> dynamicStates; 
        dynamicStates.emplace_back( VK_DYNAMIC_STATE_VIEWPORT );
        dynamicStates.emplace_back( VK_DYNAMIC_STATE_SCISSOR );
        
        VkPipelineDynamicStateCreateInfo dynamicState { };
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>( dynamicStates.size() );
        dynamicState.pDynamicStates = dynamicStates.data();

        // Setting up pipeline layout
        VkPipelineLayoutCreateInfo layoutInfo { };
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.setLayoutCount = 0;
        layoutInfo.pushConstantRangeCount = 0;

        // Create the pipeline layout
        if ( VK_SUCCESS != vkCreatePipelineLayout( device, 
                                                   &layoutInfo,
                                                   nullptr,
                                                   &pipelineLayout ) )
        {
            MSG("Failed to create pipeline layout...");
            return false;
        }

        // Create the pipeline
        VkGraphicsPipelineCreateInfo pipelineInfo { };
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multiSampling;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        if ( VK_SUCCESS != vkCreateGraphicsPipelines( device,
                                                      VK_NULL_HANDLE,
                                                      1,
                                                      &pipelineInfo,
                                                      nullptr,
                                                      &graphicsPipeline ) )
        {
            MSG("Failed to create graphics pipeline...");
            return false;
        }

        vkDestroyShaderModule( device, vertShaderModule, nullptr);
        vkDestroyShaderModule( device, fragShaderModule, nullptr);

        MSG("Created graphics pipeline!");

        return true;
    }

    VkPipeline& getGraphicsPipeline( void )
    {
        return graphicsPipeline;
    }

    bool initialize( void )
    {
        return false;
    }

    void shutdown( VkDevice device )
    {
        vkDestroyPipeline( device, graphicsPipeline, nullptr );
        vkDestroyPipelineLayout( device, pipelineLayout, nullptr );
    }

} // namespace muggy::graphics::vulkan::pipeline
