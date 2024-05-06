//********************************************************************
//  File:    vulkanGraphicsPipeline.cpp
//  Date:    Thu, 28 Mar 2024: 17:14
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************
#if USE_OLD_GRAPHICS_PIPELINE

#include "oldVulkanGraphicsPipeline.h"
#include "../vulkanShaders.h"
#include "../vulkanVertex.h"
#include "../vulkanHelpers.h"

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
        m_Config.vertexInputInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        m_Config.inputAssemblyInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        m_Config.tesselationInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
        m_Config.viewportInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        m_Config.rasterizerInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        m_Config.multiSampleInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        m_Config.colorBlendInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        m_Config.depthStencilInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        m_Config.dynamicStateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };

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

    //****************************************************************
    // vulkan_pipeline implementation
    vulkan_pipeline::vulkan_pipeline( VkDevice& device,
                                      const vulkan_pipeline_config& config )
     : 
        m_Device( device )
    {
        // Make sure the device actually exists
        assert( VK_NULL_HANDLE != m_Device );

        bool result { createGraphicsPipeline( config ) };
        if ( !result )
        {
            // Something went wrong and we need to abort
            // call release
            release();
            MSG("Failed to create pipeline...");
        }
    }

    vulkan_pipeline::~vulkan_pipeline( )
    {
        // Call release 
        release();
        // Then we add an assert here
        assert( VK_NULL_HANDLE != m_Pipeline );
        // assert( VK_NULL_HANDLE != m_Layout );
    }

    // bool 
    // vulkan_pipeline::createPipelineLayout( const utils::vector<VkDescriptorSetLayout>& setLayouts,
    //                                        const utils::vector<VkPushConstantRange>& pushConstants )
    // {
    //     // Assert for an already existing layout
    //     assert( VK_NULL_HANDLE == m_Layout );

    //     VkResult result { VK_SUCCESS };
    //     VkPipelineLayoutCreateInfo layoutInfo { };
    //     layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    //     layoutInfo.pNext = nullptr;
    //     layoutInfo.flags = 0; // Not currently in use
    //     if ( setLayouts.size( ) != 0 )
    //     {
    //         layoutInfo.setLayoutCount = static_cast<uint32_t>( setLayouts.size() );
    //         layoutInfo.pSetLayouts = setLayouts.data();
    //     }
    //     else
    //     {
    //         // Setting default values
    //         layoutInfo.setLayoutCount = 0;
    //         layoutInfo.pSetLayouts = nullptr;
    //     }
    //     if ( pushConstants.size() != 0 )
    //     {
    //         layoutInfo.pushConstantRangeCount = static_cast<uint32_t>( pushConstants.size() );
    //         layoutInfo.pPushConstantRanges = pushConstants.data();
    //     }
    //     else
    //     {
    //         // Setting defaults
    //         layoutInfo.pushConstantRangeCount = 0; // size of push constant ranges
    //         layoutInfo.pPushConstantRanges = nullptr; // vector of push contant ranges
    //     }

    //     // Create the pipeline layout
    //     result = vkCreatePipelineLayout( m_Device,
    //                                      &layoutInfo,
    //                                      nullptr,
    //                                      &m_Layout );
    //     if ( VK_SUCCESS != result )
    //     {
    //         MSG("Failed to create pipeline layout...");
    //         return false;
    //     }

    // }

    bool vulkan_pipeline::createGraphicsPipeline( const vulkan_pipeline_config& config )
    {
        // Make sure there is a layout defined in the config
        assert( VK_NULL_HANDLE != config.layout );
        // Make sure there is a render pass defined in the  config
        assert( VK_NULL_HANDLE != config.renderPass );

        VkResult result { VK_SUCCESS };
        VkShaderModule vertShaderModule { };
        VkShaderModule fragShaderModule { };

        // TODO(klek): Take these shaders as input that we can request
        //             from the shader library instead
        // Create the shader modules from sources
        if ( shaders::createShaderModule( m_Device,
                                          shaders::engine_shader::triangleShaderVS,
                                          vertShaderModule ) )
        {
            MSG("Failed to create shader module...");
            return false;
        }
        if ( shaders::createShaderModule( m_Device,
                                          shaders::engine_shader::triangleShaderFS,
                                          fragShaderModule ) )
        {
            MSG("Failed to create shader module...");
            return false;
        }
        // Set up the shader stages
        utils::vector<VkPipelineShaderStageCreateInfo> shaderStages { };
        shaderStages.push_back( vulkan_utils::setShaderStageInfo( VK_SHADER_STAGE_VERTEX_BIT,
                                                                  vertShaderModule,
                                                                  "main" ) );
        shaderStages.push_back( vulkan_utils::setShaderStageInfo( VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                  fragShaderModule,
                                                                  "main" ) );
        // Setup the vertex input state
        VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
        vertexInputInfo = vulkan_utils::setVertexInputInfo( config.bindingDescription,
                                                            config.attributeDescription );
        // Setup the pipeline layout
        VkPipelineLayoutCreateInfo layoutInfo { };

        // Setup the pipeline info struct
        VkGraphicsPipelineCreateInfo pipelineInfo { };
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = nullptr;
        pipelineInfo.flags = 0;
        pipelineInfo.stageCount = static_cast<uint32_t>( shaderStages.size() );
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &config.inputAssemblyInfo;
        pipelineInfo.pTessellationState = nullptr; // &config.tesselationInfo;
        pipelineInfo.pViewportState = &config.viewportInfo;
        pipelineInfo.pRasterizationState = &config.rasterizerInfo;
        pipelineInfo.pMultisampleState = &config.multiSampleInfo;
        pipelineInfo.pDepthStencilState = &config.depthStencilInfo;
        pipelineInfo.pColorBlendState = &config.colorBlendInfo;
        pipelineInfo.pDynamicState = &config.dynamicStateInfo;
        // TODO(klek): Set up the pipeline layout
        pipelineInfo.layout = config.layout;
        // TODO(klek): We need a render pass
        pipelineInfo.renderPass = config.renderPass;
        // TODO(klek): We need a subpass
        pipelineInfo.subpass = config.subPass;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        result = vkCreateGraphicsPipelines( m_Device,
                                            VK_NULL_HANDLE,
                                            1,
                                            &pipelineInfo,
                                            nullptr,
                                            &m_Pipeline );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to create graphics pipeline...");
            return false;
        }

        MSG("Created a graphics pipeline!");

        // Destroy the shader modules
        vkDestroyShaderModule( m_Device, vertShaderModule, nullptr );
        vkDestroyShaderModule( m_Device, fragShaderModule, nullptr );

        return true;
    }

    void vulkan_pipeline::release( void )
    {
        if ( VK_NULL_HANDLE != m_Pipeline )
        {
            vkDestroyPipeline( m_Device, m_Pipeline, nullptr );
        }
        // if ( VK_NULL_HANDLE != m_Layout )
        // {
        //     vkDestroyPipelineLayout( m_Device, m_Layout, nullptr );
        // }
        // NOTE(klek): We don't do anything with the m_Device variable
        //             since we don't own this and it belongs to
        //             whoever called us in the beginning
    }

    void vulkan_pipeline::bind( VkCommandBuffer cmdBuffer )
    {
        vkCmdBindPipeline( cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline );
    }

    void vulkan_pipeline::defaultPipelineConfigInfo( vulkan_pipeline_config& config )
    {
        // First make sure that the vector components are zero
        assert( config.bindingDescription.size() != 0 );
        assert( config.attributeDescription.size() != 0 );
        assert( config.dynamicStates.size() != 0 );
        assert( config.colorBlendAttachment.size() != 0 );

        // TODO(klek): Populating the binding and attribute descriptions
        //             based on a model data
        config.bindingDescription = vertex::vulkan_vertex::getBindingDescriptions( 0 );
        config.attributeDescription = vertex::vulkan_vertex::getAttributeDescriptions( 0 );

        // Setup dynamic states
        config.dynamicStates.push_back( VK_DYNAMIC_STATE_VIEWPORT );
        config.dynamicStates.push_back( VK_DYNAMIC_STATE_SCISSOR );

        // Setup color blend attachments, which should use default here
        config.colorBlendAttachment.push_back( vulkan_utils::defaultColorBlendAttachmentInfo( ) );
        
        // Shader stage info and vertex input info depend on shaders
        // which there currently are none that are default and thus 
        // these members are not populated here
        // config.shaderStageInfo = defaultShaderStageInfo( );
        // config.vertexInputInfo = defaultVertexInputInfo( );

        // Use default input assembly
        config.inputAssemblyInfo = vulkan_utils::defaultInputAssemblyInfo( );
        // Use default viewport which assumes dynamic states are set
        config.viewportInfo = vulkan_utils::defaultViewportInfo( );
        // Use default rasterizer
        config.rasterizerInfo = vulkan_utils::defaultRasterizerInfo( );
        // Use default multi sample
        config.multiSampleInfo = vulkan_utils::defaultMultiSampleInfo( );
        // Set color blend info based on the default color blend attachment
        config.colorBlendInfo = vulkan_utils::setColorBlendInfo( config.colorBlendAttachment );
        // Use default depth stencil
        config.depthStencilInfo = vulkan_utils::defaultDepthStencilInfo( );
        // Set the dynamic states info based on the dynamic states specified
        config.dynamicStateInfo = vulkan_utils::setDynamicStateInfo( config.dynamicStates );
    }

    //****************************************************************
    // 

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
            vulkan_utils::setShaderStageInfo( VK_SHADER_STAGE_VERTEX_BIT,
                                              vertShaderModule,
                                              "main" )
        };

        VkPipelineShaderStageCreateInfo fShaderStageInfo {
            vulkan_utils::setShaderStageInfo( VK_SHADER_STAGE_FRAGMENT_BIT,
                                              fragShaderModule,
                                              "main" )
        };

        utils::vector<VkPipelineShaderStageCreateInfo> shaderStages {};
        shaderStages.push_back( vShaderStageInfo );
        shaderStages.push_back( fShaderStageInfo );

        auto bindingDescription = vertex::vulkan_vertex::getBindingDescriptions( 0 );
        auto attributeDescription = vertex::vulkan_vertex::getAttributeDescriptions( 0 );
        // Setting up vertex input
        VkPipelineVertexInputStateCreateInfo vertexInputInfo { };
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.pNext = nullptr;
        vertexInputInfo.flags = 0;
        // vertexInputInfo.vertexBindingDescriptionCount = 0;
        // vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>( bindingDescription.size() );
        vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();
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

#endif