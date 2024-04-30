//********************************************************************
//  File:    vulkanGraphicsPipeline.h
//  Date:    Thu, 28 Mar 2024: 17:13
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_PIPELINE_H)
#define VULKAN_PIPELINE_H

#include "vulkanCommon.h"

namespace muggy::graphics::vulkan::pipeline
{
    struct vulkan_pipeline_config
    {
        vulkan_pipeline_config() = default;
        MUGGY_DISABLE_COPY_AND_MOVE( vulkan_pipeline_config );

        // Vectors in this struct
        utils::vector<VkVertexInputBindingDescription>      bindingDescription { };
        utils::vector<VkVertexInputAttributeDescription>    attributeDescription { };
        utils::vector<VkDynamicState>                       dynamicStates { };
        utils::vector<VkPipelineColorBlendAttachmentState>  colorBlendAttachment { };
        // Usually shaderstages are multiple (vertex + fragment)
        utils::vector<VkPipelineShaderStageCreateInfo>      shaderStageInfos { };
        // Create info structures
        VkPipelineVertexInputStateCreateInfo                vertexInputInfo { };
        VkPipelineInputAssemblyStateCreateInfo              inputAssemblyInfo { };
        VkPipelineTessellationStateCreateInfo               tesselationInfo { };
        VkPipelineViewportStateCreateInfo                   viewportInfo { };
        VkPipelineRasterizationStateCreateInfo              rasterizerInfo { };
        VkPipelineMultisampleStateCreateInfo                multiSampleInfo { };
        // VkPipelineColorBlendAttachmentState                 colorBlendAttachment { };
        VkPipelineColorBlendStateCreateInfo                 colorBlendInfo { };
        VkPipelineDepthStencilStateCreateInfo               depthStencilInfo { };
        VkPipelineDynamicStateCreateInfo                    dynamicStateInfo { };

        // Additional parameters needed
        VkPipelineLayout                                    layout { VK_NULL_HANDLE };
        VkRenderPass                                        renderPass { VK_NULL_HANDLE };
        uint32_t                                            subPass { 0 };
    };

    // Builder class to setup and build a pipeline
    // NOTE(klek): This class does not own the pipeline layout, the 
    //             renderpass or the actual pipeline created!!
    //             The layout and renderpass has to be provided to
    //             properly build the pipeline
    class vulkan_pipeline_builder
    {
    public:
        vulkan_pipeline_builder() { clear(); }
        MUGGY_DISABLE_COPY_AND_MOVE( vulkan_pipeline_builder );

        bool buildPipeline( VkDevice device, VkPipeline& pipeline );
        void clear( void );

        // Set functions for the layout and renderpass
        void setLayout( const VkPipelineLayout& layout );
        void setRenderPass( const VkRenderPass& renderPass, uint32_t subPass );

        // Set functions to configure the different stages in the
        // pipeline config
        void setShaderStages( VkShaderModule vert, VkShaderModule frag );
        void setVertexBindings( const utils::vector<VkVertexInputBindingDescription>& bindings );
        void setVertexAttributes( const utils::vector<VkVertexInputAttributeDescription>& attributes );
        void setDynamicState( VkDynamicState state );
        void setDynamicViewPort( void );
        void setDynamicScissor( void );
        void setInputTopology( VkPrimitiveTopology topology, VkBool32 restartEnable = VK_FALSE );
        void setPolygonMode( VkPolygonMode mode );
        void setCullMode( VkCullModeFlags cullMode, VkFrontFace frontFace );
        void setLineWidth( float lineWidth = 1.0f );
        void disableMultiSampling( void );
        void setBlendingAdditive( void );
        void setBlendingAlphaBlend( void );
        void disableBlending( );
        void setColorAttachmentFormat( VkFormat format );
        void setDepthFormat( VkFormat format );
        void disableDepthTesting( );
        void enableDepthTesting( bool depthWriteEnable, VkCompareOp op );

    private:
        vulkan_pipeline_config m_Config { };
    };

} // namespace muggy::graphics::vulkan::pipeline



#endif