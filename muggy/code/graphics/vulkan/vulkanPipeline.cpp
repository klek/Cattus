//********************************************************************
//  File:    vulkanGraphicsPipeline.cpp
//  Date:    Thu, 28 Mar 2024: 17:14
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanPipeline.h"
#include "vulkanShaders.h"
#include "vulkanVertex.h"
#include "vulkanHelpers.h"

namespace muggy::graphics::vulkan::pipeline
{
    namespace 
    {
        // TODO(klek): these should likely be part of a class
        //VkPipelineLayout pipelineLayout;
        //VkPipeline       graphicsPipeline;
    } // namespace anonymous

    //****************************************************************
    // vulkan_pipeline_builder implementation
    bool vulkan_pipeline_builder::buildPipeline( VkDevice device, VkPipeline& pipeline )
    {
        // Check that the provided pipeline is null
        assert( VK_NULL_HANDLE == pipeline );
        // Check that internal variable has a valid layout
        assert( VK_NULL_HANDLE != m_Config.layout );
        // Check that we have provided a valid renderpass
        assert( VK_NULL_HANDLE != m_Config.renderPass );

        // Local variables
        VkResult result { VK_SUCCESS };

        // Set the vertexInputInfo struct
        m_Config.vertexInputInfo = vulkan_utils::setVertexInputInfo( m_Config.bindingDescription,
                                                                     m_Config.attributeDescription );

        // Set default viewport
        m_Config.viewportInfo = vulkan_utils::defaultViewportInfo( );

        // Set the dynamic states info
        m_Config.dynamicStateInfo = vulkan_utils::setDynamicStateInfo( m_Config.dynamicStates );

        // Set the color blend states info
        m_Config.colorBlendInfo = vulkan_utils::setColorBlendInfo( m_Config.colorBlendAttachment );

        // Setup the pipeline info struct
        VkGraphicsPipelineCreateInfo pipelineInfo { };
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = nullptr;
        pipelineInfo.flags = 0;
        pipelineInfo.stageCount = static_cast<uint32_t>( m_Config.shaderStageInfos.size() );
        pipelineInfo.pStages = m_Config.shaderStageInfos.data();
        pipelineInfo.pVertexInputState = &m_Config.vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &m_Config.inputAssemblyInfo;
        pipelineInfo.pTessellationState = nullptr; // &m_Config.tesselationInfo;
        pipelineInfo.pViewportState = &m_Config.viewportInfo;
        pipelineInfo.pRasterizationState = &m_Config.rasterizerInfo;
        pipelineInfo.pMultisampleState = &m_Config.multiSampleInfo;
        pipelineInfo.pDepthStencilState = &m_Config.depthStencilInfo;
        pipelineInfo.pColorBlendState = &m_Config.colorBlendInfo;
        pipelineInfo.pDynamicState = &m_Config.dynamicStateInfo;
        // TODO(klek): Set up the pipeline layout
        pipelineInfo.layout = m_Config.layout;
        // TODO(klek): We need a render pass
        pipelineInfo.renderPass = m_Config.renderPass;
        // TODO(klek): We need a subpass
        pipelineInfo.subpass = m_Config.subPass;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        VkPipeline newPipeline { VK_NULL_HANDLE };
        result = vkCreateGraphicsPipelines( device,
                                            VK_NULL_HANDLE,
                                            1,
                                            &pipelineInfo,
                                            nullptr,
                                            &newPipeline );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to create graphics pipeline...");
            return false;
        }
        MSG("Created a graphics pipeline!");
        pipeline = newPipeline;

        return true;
    }

    // Clear function that sets the member variable to default values!
    void vulkan_pipeline_builder::clear( void )
    {
        // NOTE(klek): Since this class shouldn't own these, should
        //             we really check them?
        // Ideally neither layout nor renderpass should be set
        // for now, assert if they are...
        assert( VK_NULL_HANDLE == m_Config.layout );
        assert( VK_NULL_HANDLE == m_Config.renderPass );

        // Clear all of the vectors
        m_Config.bindingDescription.clear();
        m_Config.attributeDescription.clear();
        m_Config.dynamicStates.clear();
        m_Config.colorBlendAttachment.clear();
        m_Config.shaderStageInfos.clear();

        // Set all of the structs to their default initialized value
        // but with their type being set
        m_Config.vertexInputInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        m_Config.inputAssemblyInfo = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        m_Config.tesselationInfo = { VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
        m_Config.viewportInfo = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        m_Config.rasterizerInfo = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        m_Config.multiSampleInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        m_Config.colorBlendInfo = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        m_Config.depthStencilInfo = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        m_Config.dynamicStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };

        // Set subpass to 0
        m_Config.subPass = 0;
    }

    // Set functions for the layout and renderpass
    void vulkan_pipeline_builder::setLayout( const VkPipelineLayout& layout )
    {
        // Should we error check the input here?
        m_Config.layout = layout;
    }

    void vulkan_pipeline_builder::setRenderPass( const VkRenderPass& renderPass,
                                                 uint32_t subPass )
    {
        // Should we error check the input here?
        m_Config.renderPass = renderPass;
        m_Config.subPass = subPass;
    }

    void vulkan_pipeline_builder::setShaderStages( VkShaderModule vert,
                                                   VkShaderModule frag )
    {
        // Make sure our shaderStages vector is cleared
        m_Config.shaderStageInfos.clear();

        // NOTE(klek): Current we are hardcoding the stage flag and
        //             the entry point
        m_Config.shaderStageInfos.push_back( vulkan_utils::setShaderStageInfo( VK_SHADER_STAGE_VERTEX_BIT,
                                                                               vert,
                                                                               "main" ) );
        m_Config.shaderStageInfos.push_back( vulkan_utils::setShaderStageInfo( VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                               frag,
                                                                               "main" ) );
    }
   
    void vulkan_pipeline_builder::setVertexBindings( const utils::vector<VkVertexInputBindingDescription>& bindings )
    {
        // TODO(klek): Implement this
        for ( const VkVertexInputBindingDescription& desc : bindings )
        {
            m_Config.bindingDescription.push_back( desc );
        }
    }

    void vulkan_pipeline_builder::setVertexAttributes( const utils::vector<VkVertexInputAttributeDescription>& attributes )
    {
        for ( const VkVertexInputAttributeDescription& desc : attributes )
        {
            m_Config.attributeDescription.push_back( desc );
        }
    }

    void vulkan_pipeline_builder::setDynamicState( VkDynamicState state )
    {
        m_Config.dynamicStates.push_back( state );
    }

    void vulkan_pipeline_builder::setDynamicViewPort( void )
    {
        setDynamicState( VK_DYNAMIC_STATE_VIEWPORT ); 
    }

    void vulkan_pipeline_builder::setDynamicScissor( void )
    {
        setDynamicState( VK_DYNAMIC_STATE_SCISSOR ); 
    }

    void vulkan_pipeline_builder::setInputTopology( VkPrimitiveTopology topology,
                                                    VkBool32 restartEnable /*= VK_FALSE*/ )
    {
        // Setting the assembly info struct
        m_Config.inputAssemblyInfo.topology = topology;
        m_Config.inputAssemblyInfo.primitiveRestartEnable = restartEnable;
    }

    void vulkan_pipeline_builder::setPolygonMode( VkPolygonMode mode )
    {
        // Set rasterizer polygon mode
        m_Config.rasterizerInfo.polygonMode = mode;
    }

    void vulkan_pipeline_builder::setCullMode( VkCullModeFlags cullMode, VkFrontFace frontFace )
    {
        // Set rasterizer cull mode
        m_Config.rasterizerInfo.cullMode = cullMode;
        m_Config.rasterizerInfo.frontFace = frontFace;
    }

    void vulkan_pipeline_builder::setLineWidth( float lineWidth /*= 1.0f */ )
    {
        m_Config.rasterizerInfo.lineWidth = lineWidth;
    }

    void vulkan_pipeline_builder::disableMultiSampling(  )
    {
        // TODO(klek): Implement this
        m_Config.multiSampleInfo = vulkan_utils::defaultMultiSampleInfo();
    }

    void vulkan_pipeline_builder::setBlendingAdditive( )
    {
        // NOTE(klek): Using default structure
        VkPipelineColorBlendAttachmentState colorBlend { vulkan_utils::defaultColorBlendAttachmentInfo() };
        colorBlend.blendEnable = VK_TRUE;
        colorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
        colorBlend.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlend.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
                                    VK_COLOR_COMPONENT_G_BIT | 
                                    VK_COLOR_COMPONENT_B_BIT | 
                                    VK_COLOR_COMPONENT_A_BIT;
        // Push this into our vector
        m_Config.colorBlendAttachment.push_back( colorBlend );
    }

    void vulkan_pipeline_builder::setBlendingAlphaBlend( )
    {
        // NOTE(klek): Using default structure
        VkPipelineColorBlendAttachmentState colorBlend { vulkan_utils::defaultColorBlendAttachmentInfo() };
        colorBlend.blendEnable = VK_TRUE;
        colorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        colorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
        colorBlend.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlend.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
                                    VK_COLOR_COMPONENT_G_BIT | 
                                    VK_COLOR_COMPONENT_B_BIT | 
                                    VK_COLOR_COMPONENT_A_BIT;
        // Push this into our vector
        m_Config.colorBlendAttachment.push_back( colorBlend );
    }

    void vulkan_pipeline_builder::disableBlending( )
    {
        // Using default structure for disabling of blending
        VkPipelineColorBlendAttachmentState colorBlend { vulkan_utils::defaultColorBlendAttachmentInfo() };
        m_Config.colorBlendAttachment.push_back( colorBlend );
    }

    void vulkan_pipeline_builder::setColorAttachmentFormat( VkFormat format )
    {
        // TODO(klek): Implement this
    }

    void vulkan_pipeline_builder::setDepthFormat( VkFormat format )
    {
        // TODO(klek): Implement this
    }

    void vulkan_pipeline_builder::disableDepthTesting( )
    {
        m_Config.depthStencilInfo.pNext = nullptr;
        m_Config.depthStencilInfo.flags = 0;
        m_Config.depthStencilInfo.depthTestEnable = VK_FALSE;
        m_Config.depthStencilInfo.depthWriteEnable = VK_FALSE;
        m_Config.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_NEVER;
        m_Config.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        m_Config.depthStencilInfo.stencilTestEnable = VK_FALSE;
        m_Config.depthStencilInfo.front = { };
        m_Config.depthStencilInfo.back = { };
        m_Config.depthStencilInfo.minDepthBounds = 0.0f;
        m_Config.depthStencilInfo.maxDepthBounds = 1.0f;
    }

    void vulkan_pipeline_builder::enableDepthTesting( bool depthWriteEnable, VkCompareOp op )
    {
        m_Config.depthStencilInfo.pNext = nullptr;
        m_Config.depthStencilInfo.flags = 0;
        m_Config.depthStencilInfo.depthTestEnable = VK_TRUE;
        m_Config.depthStencilInfo.depthWriteEnable = depthWriteEnable;
        m_Config.depthStencilInfo.depthCompareOp = op;
        m_Config.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        m_Config.depthStencilInfo.stencilTestEnable = VK_FALSE;
        m_Config.depthStencilInfo.front = { };
        m_Config.depthStencilInfo.back = { };
        m_Config.depthStencilInfo.minDepthBounds = 0.0f;
        m_Config.depthStencilInfo.maxDepthBounds = 1.0f;
    }

} // namespace muggy::graphics::vulkan::pipeline
