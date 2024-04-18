//********************************************************************
//  File:    vulkanGraphicsPipeline.cpp
//  Date:    Thu, 28 Mar 2024: 17:14
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanGraphicsPipeline.h"
#include "vulkanShaders.h"
#include "vulkanVertex.h"

namespace muggy::graphics::vulkan::pipeline
{
    namespace 
    {
        // TODO(klek): these should likely be part of a class
        //VkPipelineLayout pipelineLayout;
        //VkPipeline       graphicsPipeline;

        VkPipelineShaderStageCreateInfo
        getShaderCreateInfo( VkShaderStageFlagBits stage,
                             VkShaderModule module,
                             const char* name )
        {
            VkPipelineShaderStageCreateInfo shaderStageInfo { };
            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.pNext = nullptr;
            shaderStageInfo.flags = 0;
            shaderStageInfo.stage = stage;
            shaderStageInfo.module = module;
            shaderStageInfo.pName = name;
            shaderStageInfo.pSpecializationInfo = nullptr;

            return shaderStageInfo;
        }
    } // namespace anonymous
    
    bool createGraphicsPipeline( VkDevice device, VkRenderPass renderPass,
                                 VkPipeline& graphicsPipeline,
                                 VkPipelineLayout& pipelineLayout )
    {
        // Only 1 pipeline and pipelinelayout
        if ( graphicsPipeline && pipelineLayout )
        {
            return true;
        }
        VkShaderModule vertShaderModule { };
        VkShaderModule fragShaderModule { };

        // TODO(klek): Add error handling to shader module creation
        shaders::createShaderModule( device, shaders::engine_shader::triangleShaderVS, vertShaderModule );
        shaders::createShaderModule( device, shaders::engine_shader::triangleShaderFS, fragShaderModule );

        // Setting up shaderstages
        VkPipelineShaderStageCreateInfo vShaderStageInfo {
            getShaderCreateInfo( VK_SHADER_STAGE_VERTEX_BIT,
                                 vertShaderModule,
                                 "main" )
        };

        VkPipelineShaderStageCreateInfo fShaderStageInfo {
            getShaderCreateInfo( VK_SHADER_STAGE_FRAGMENT_BIT,
                                 fragShaderModule,
                                 "main" )
        };

        utils::vector<VkPipelineShaderStageCreateInfo> shaderStages {};
        shaderStages.push_back( vShaderStageInfo );
        shaderStages.push_back( fShaderStageInfo );

        auto bindingDescription = vertex::vulkan_vertex::getBindingDescription();
        auto attributeDescription = vertex::vulkan_vertex::getAttributeDescription();
        // Setting up vertex input
        VkPipelineVertexInputStateCreateInfo vertexInputInfo { };
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.pNext = nullptr;
        vertexInputInfo.flags = 0;
        // vertexInputInfo.vertexBindingDescriptionCount = 0;
        // vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>( attributeDescription.size() );
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

        // Setting up assembly input
        VkPipelineInputAssemblyStateCreateInfo inputAssembly { };
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.pNext = nullptr;
        inputAssembly.flags = 0;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Setting up viewport stage
        VkPipelineViewportStateCreateInfo viewportState { };
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.pNext = nullptr;
        viewportState.flags = 0;
        viewportState.viewportCount = 1;
        // NOTE(klek): Using dynamic states for viewports
        viewportState.pViewports = nullptr;
        viewportState.scissorCount = 1;
        // NOTE(klek): Using dynamic states for scissors
        viewportState.pScissors = nullptr;

        // Setting up rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer { };
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.pNext = nullptr;
        rasterizer.flags = 0;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;
        rasterizer.lineWidth = 1.0f;

        // Setting up multisampling
        VkPipelineMultisampleStateCreateInfo multiSampling { };
        multiSampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multiSampling.pNext = nullptr;
        multiSampling.flags = 0;
        multiSampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multiSampling.sampleShadingEnable = VK_FALSE;
        multiSampling.minSampleShading = 1.0f;
        multiSampling.pSampleMask = nullptr;
        multiSampling.alphaToCoverageEnable = VK_FALSE;
        multiSampling.alphaToOneEnable = VK_FALSE;

        // Setting up colorblend
        VkPipelineColorBlendAttachmentState colorBlendAttachment { };
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
                                              VK_COLOR_COMPONENT_G_BIT | 
                                              VK_COLOR_COMPONENT_B_BIT | 
                                              VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlending { };
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.pNext = nullptr;
        colorBlending.flags = 0;
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
        dynamicState.pNext = nullptr;
        dynamicState.flags = 0;
        dynamicState.dynamicStateCount = static_cast<uint32_t>( dynamicStates.size() );
        dynamicState.pDynamicStates = dynamicStates.data();

        // Setting up pipeline layout
        VkPipelineLayoutCreateInfo layoutInfo { };
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.pNext = nullptr;
        layoutInfo.flags = 0;
        layoutInfo.setLayoutCount = 0;
        layoutInfo.pSetLayouts = nullptr;
        layoutInfo.pushConstantRangeCount = 0;
        layoutInfo.pPushConstantRanges = nullptr;

        // Create the pipeline layout
        VkResult result { VK_SUCCESS };
        result = vkCreatePipelineLayout( device,
                                         &layoutInfo,
                                         nullptr,
                                         &pipelineLayout );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to create pipeline layout...");
            return false;
        }

        // Create the pipeline
        VkGraphicsPipelineCreateInfo pipelineInfo { };
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = nullptr;
        pipelineInfo.flags = 0;
        pipelineInfo.stageCount = static_cast<uint32_t>( shaderStages.size() );
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pTessellationState = nullptr;
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

        result = vkCreateGraphicsPipelines( device,
                                            VK_NULL_HANDLE,
                                            1,
                                            &pipelineInfo,
                                            nullptr,
                                            &graphicsPipeline );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to create graphics pipeline...");
            return false;
        }

        // NOTE(klek): Destroying shader modules here since they are
        //             no longer needed.
        // TODO(klek): Consider adding smart-pointer functionality to
        //             these shader modules, such that they are
        //             automatically destroyed when they go out of scope
        vkDestroyShaderModule( device, vertShaderModule, nullptr);
        vkDestroyShaderModule( device, fragShaderModule, nullptr);

        MSG("Created graphics pipeline!");

        return true;
    }

    // VkPipeline& getGraphicsPipeline( void )
    // {
    //     return graphicsPipeline;
    // }

    // bool initialize( void )
    // {
    //     return false;
    // }

    // void shutdown( VkDevice device )
    // {
    //     vkDestroyPipeline( device, graphicsPipeline, nullptr );
    //     vkDestroyPipelineLayout( device, pipelineLayout, nullptr );
    // }

    void destroyGraphicsPipeline( VkDevice device,
                                  VkPipeline& graphicsPipeline,
                                  VkPipelineLayout& pipelineLayout )
    {
        vkDestroyPipeline( device, graphicsPipeline, nullptr );
        vkDestroyPipelineLayout( device, pipelineLayout, nullptr );
    }

} // namespace muggy::graphics::vulkan::pipeline
