//********************************************************************
//  File:    vulkanHelpers.h
//  Date:    Sun, 14 Apr 2024: 18:13
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_HELPERS_H)
#define VULKAN_HELPERS_H

#include "vulkanCommon.h"

namespace muggy::graphics::vulkan::vulkan_utils
{
    //****************************************************************
    // Instance creation
    VkApplicationInfo setApplicationInfo( uint32_t appVersion,
                                          uint32_t engineVersion,
                                          uint32_t apiVersion,
                                          const char* appName = "Application name",
                                          const char* engineName = "MuggyEngine" );

    VkInstanceCreateInfo setInstanceCreateInfo( const VkApplicationInfo& appInfo,
                                                const utils::vector<const char*>& ext );

    //****************************************************************
    // Commandpool and commandbuffers
    VkCommandPoolCreateInfo setCmdPoolCreateInfo( const uint32_t queueFamilyIdx,
                                                  const VkCommandPoolCreateFlags flags = 0 );

    VkCommandBufferAllocateInfo setCmdBufferCreateInfo( const VkCommandPool& cmdPool,
                                                        const VkCommandBufferLevel level,
                                                        const uint32_t count = 1 );

    VkCommandBufferBeginInfo setCmdBufferBeginInfo( const VkCommandBufferUsageFlags flags = 0 );

    VkCommandBufferSubmitInfo setCmdBufferSubmitInfo( const VkCommandBuffer& cmdBuffer );

    //****************************************************************
    // Device creation
    VkDeviceQueueCreateInfo setQueueCreateInfo( const uint32_t& queueFamilyIdx,
                                                const uint32_t& queueCount,
                                                const VkDeviceQueueCreateFlags flags = 0 );

    VkDeviceCreateInfo setDeviceCreateInfo( const utils::vector<VkDeviceQueueCreateInfo>& queueInfos,
                                            const utils::vector<const char*>& deviceExt,
                                            const utils::vector<const char*>& layers = {},
                                            const VkPhysicalDeviceFeatures& deviceFeat = {} );

    //****************************************************************
    // Synchronization structures
    VkFenceCreateInfo setFenceCreateInfo( const VkFenceCreateFlags flags = 0 );

    VkSemaphoreCreateInfo setSemaphoreCreateInfo( const VkSemaphoreCreateFlags flags = 0 );

    VkSemaphoreSubmitInfo SetSemaphoreSubmitInfo( const VkPipelineStageFlags2 stageMask,
                                                  const VkSemaphore& sem );

    //****************************************************************
    // Presentation
    VkPresentInfoKHR setPresentInfo( const VkSwapchainKHR* swapchain,
                                     const VkSemaphore* waitSem,
                                     const uint32_t* imageIndex );

    VkSubmitInfo setSubmitInfo( const VkCommandBuffer& cmdBuffer,
                                const VkSemaphore& imageAvailable,
                                const VkSemaphore& renderFinished,
                                const VkPipelineStageFlags* waitStage = nullptr );

    //****************************************************************
    // Renderpass
    // NOTE(klek): This function does not set the clear color!!
    VkRenderPassBeginInfo setRenderpassBeginInfo( const VkRenderPass& renderPass,
                                                  const VkFramebuffer& frameBuffer,
                                                  VkRect2D renderArea );

    //****************************************************************
    // Buffers
    VkBufferCreateInfo bufferCreateInfo( VkBufferUsageFlags usageFlags,
                                         VkDeviceSize size,
                                         VkSharingMode mode );

    VkMemoryAllocateInfo memoryAllocationInfo( void );

    //****************************************************************
    // Images
    VkImageViewCreateInfo setImageViewInfo( const VkImage& image,
                                            VkFormat format,
                                            VkImageAspectFlags aspectFlags );

    VkImageSubresourceRange imageSubresourceRange( const VkImageAspectFlags& aspectMask );

    void transitionImage( const VkCommandBuffer& cmdBuffer,
                          const VkImage& image,
                          const VkImageLayout currentLayout,
                          const VkImageLayout newLayout );

    //****************************************************************
    // Pipeline related
    // Helper functions to set create state info structs
    VkPipelineShaderStageCreateInfo
    setShaderStageInfo( VkShaderStageFlagBits stage,
                        VkShaderModule module,
                        const char* name );
    
    VkPipelineVertexInputStateCreateInfo
    setVertexInputInfo( const utils::vector<VkVertexInputBindingDescription>& bindingDesc,
                        const utils::vector<VkVertexInputAttributeDescription>& attributeDesc );
    
    VkPipelineTessellationStateCreateInfo
    setTesselationInfo( void );
    
    VkPipelineColorBlendStateCreateInfo
    setColorBlendInfo( const utils::vector<VkPipelineColorBlendAttachmentState>& attachment );

    VkPipelineDynamicStateCreateInfo
    setDynamicStateInfo( const utils::vector<VkDynamicState>& dynamicStates );

    //****************************************************************
    // Pipeline related
    // Helper functions for default create state info structs
    VkPipelineLayoutCreateInfo
    defaultPipelineLayoutInfo( void );

    VkPipelineInputAssemblyStateCreateInfo
    defaultInputAssemblyInfo( void );
    
    VkPipelineViewportStateCreateInfo
    defaultViewportInfo( void );
    
    VkPipelineRasterizationStateCreateInfo
    defaultRasterizerInfo( void );

    VkPipelineMultisampleStateCreateInfo
    defaultMultiSampleInfo( void );

    VkPipelineColorBlendAttachmentState
    defaultColorBlendAttachmentInfo( void );

    VkPipelineDepthStencilStateCreateInfo
    defaultDepthStencilInfo( void );
    
} // namespace muggy::graphics::vulkan::vulkan_utils


#endif