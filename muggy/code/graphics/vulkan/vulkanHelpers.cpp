//********************************************************************
//  File:    vulkanHelpers.cpp
//  Date:    Mon, 22 Apr 2024: 14:13
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanHelpers.h"

namespace muggy::graphics::vulkan::vulkan_utils
{
    //****************************************************************
    // Instance creation
    VkApplicationInfo setApplicationInfo( uint32_t appVersion,
                                          uint32_t engineVersion,
                                          uint32_t apiVersion,
                                          const char* appName,
                                          const char* engineName )
    {
        VkApplicationInfo info { };
        info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        // TODO(klek): Change these values from hardcoded to
        //             application specific. Perhaps extracted from
        //             users game name
        // info.pApplicationName = "Application name";
        info.pApplicationName = appName;
        // info.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
        info.applicationVersion = appVersion;
        // TODO(klek): Change these values from hardcoded to
        //             application specific. Perhaps use defines
        //             in common.h?
        // info.pEngineName = "MuggyEngine";
        info.pEngineName = engineName;
        // info.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
        info.engineVersion = engineVersion;
        // info.apiVersion = VK_API_VERSION_1_2;
        info.apiVersion = apiVersion;

        return info;
    }

    VkInstanceCreateInfo setInstanceCreateInfo( const VkApplicationInfo& appInfo,
                                                const utils::vector<const char*>& ext )
    {
        VkInstanceCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        info.pApplicationInfo = &appInfo;
        info.enabledExtensionCount = static_cast<uint32_t>( ext.size() );
        info.ppEnabledExtensionNames = ext.data();

        return info;
    }

    //****************************************************************
    // Device creation
    VkDeviceQueueCreateInfo setQueueCreateInfo( const uint32_t& queueFamilyIdx,
                                                const uint32_t& queueCount,
                                                const VkDeviceQueueCreateFlags flags /*= 0*/ )
    {
        VkDeviceQueueCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.queueFamilyIndex = queueFamilyIdx;
        info.queueCount = queueCount;
        // Vulkan needs to know how to handle multiple queues
        // NOTE(klek): At the moment we don't handle priorities
        float prio{ 1.0f };
        info.pQueuePriorities = &prio;

        return info;
    }

    VkDeviceCreateInfo setDeviceCreateInfo( const utils::vector<VkDeviceQueueCreateInfo>& queueInfos,
                                            const utils::vector<const char*>& deviceExt,
                                            const utils::vector<const char*>& layers /*= {}*/,
                                            const VkPhysicalDeviceFeatures& deviceFeat /*= {}*/ )
    {
        VkDeviceCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.queueCreateInfoCount = static_cast<uint32_t>( queueInfos.size() );
        info.pQueueCreateInfos = queueInfos.data();
        info.enabledLayerCount = static_cast<uint32_t>( layers.size() );
        info.ppEnabledLayerNames = layers.data();
        // NOTE(klek): deviceExt is a constant from the top of this
        //             file and contains the extensions we need
        info.enabledExtensionCount = static_cast<uint32_t>( deviceExt.size() );
        info.ppEnabledExtensionNames = deviceExt.data();
        // Physical device features, that the logical device will 
        // be using
        info.pEnabledFeatures = &deviceFeat;

        return info;
    }

    //****************************************************************
    // Commandpool and commandbuffers
    VkCommandPoolCreateInfo setCmdPoolCreateInfo( const uint32_t queueFamilyIdx,
                                                  const VkCommandPoolCreateFlags flags /*= 0*/ )
    {
        VkCommandPoolCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.queueFamilyIndex = queueFamilyIdx;

        return info;
    }

    VkCommandBufferAllocateInfo setCmdBufferCreateInfo( const VkCommandPool& cmdPool,
                                                        const VkCommandBufferLevel level,
                                                        const uint32_t count /*= 1*/ )
    {
        VkCommandBufferAllocateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.pNext = nullptr;
        info.commandPool = cmdPool;
        info.level = level;
        info.commandBufferCount = count;

        return info;
    }

    VkCommandBufferBeginInfo setCmdBufferBeginInfo( const VkCommandBufferUsageFlags flags /*= 0*/ )
    {
        VkCommandBufferBeginInfo info { };
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.pInheritanceInfo = nullptr;

        return info;
    }

    VkCommandBufferSubmitInfo setCmdBufferSubmitInfo( const VkCommandBuffer& cmdBuffer )
    {
        VkCommandBufferSubmitInfo info { };
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        info.pNext = nullptr;
        info.commandBuffer = cmdBuffer;
        info.deviceMask = 0;

        return info;
    }

    //****************************************************************
    // Synchronization structures
    VkFenceCreateInfo setFenceCreateInfo( const VkFenceCreateFlags flags /*= 0*/ )
    {
        VkFenceCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;

        return info;
    }
    
    VkSemaphoreCreateInfo setSemaphoreCreateInfo( const VkSemaphoreCreateFlags flags /*= 0*/ )
    {
        VkSemaphoreCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;

        return info;
    }

    VkSemaphoreSubmitInfo SetSemaphoreSubmitInfo( const VkPipelineStageFlags2 stageMask,
                                                  const VkSemaphore& sem )
    {
        VkSemaphoreSubmitInfo info { };
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        info.pNext = nullptr;
        info.semaphore = sem;
        info.value = 1;
        info.stageMask = stageMask;
        info.deviceIndex = 0;

        return info;
    }

    //****************************************************************
    // Presentation
    VkPresentInfoKHR setPresentInfo( const VkSwapchainKHR* swapchain,
                                     const VkSemaphore* waitSem,
                                     const uint32_t* imageIndex )
    {
        VkPresentInfoKHR info { };
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.pNext = nullptr;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = waitSem;
        info.swapchainCount = 1;
        info.pSwapchains = swapchain;
        info.pImageIndices = imageIndex;
        info.pResults = nullptr;

        return info;
    }

    VkSubmitInfo setSubmitInfo( const VkCommandBuffer& cmdBuffer,
                                const VkSemaphore& imageAvailable,
                                const VkSemaphore& renderFinished,
                                const VkPipelineStageFlags* waitStage /*= nullptr*/ )
    {
        VkSubmitInfo info { };
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.pNext = nullptr;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &imageAvailable;
        info.pWaitDstStageMask = waitStage;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &cmdBuffer;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &renderFinished;

        return info;
    }

    //****************************************************************
    // Renderpass
    // NOTE(klek): This function does not set the clear color!!
    VkRenderPassBeginInfo setRenderpassBeginInfo( const VkRenderPass& renderPass,
                                                  const VkFramebuffer& frameBuffer,
                                                  VkRect2D renderArea )
    {
        VkRenderPassBeginInfo info { };
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.pNext = nullptr;
        info.renderPass = renderPass;
        info.framebuffer = frameBuffer;
        info.renderArea.offset.x = renderArea.offset.x;
        info.renderArea.offset.y = renderArea.offset.y;
        info.renderArea.extent.width = renderArea.extent.width;
        info.renderArea.extent.height = renderArea.extent.height;
        info.clearValueCount = 0;
        info.pClearValues = nullptr;

        return info;
    }

    //****************************************************************
    // Buffers
    VkBufferCreateInfo bufferCreateInfo( VkBufferUsageFlags usageFlags,
                                         VkDeviceSize size,
                                         VkSharingMode mode )
    {
        VkBufferCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.size = size;
        info.usage = usageFlags;
        info.sharingMode = mode;
        info.queueFamilyIndexCount = 0;
        info.pQueueFamilyIndices = 0;

        return info;
    }

    VkMemoryAllocateInfo memoryAllocationInfo( void )
    {
        VkMemoryAllocateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        info.pNext = nullptr;
        info.allocationSize = 0;
        info.memoryTypeIndex = 0;

        return info;
    }

    //****************************************************************
    // Images
    VkImageViewCreateInfo setImageViewInfo( const VkImage& image,
                                            VkFormat format,
                                            VkImageAspectFlags aspectFlags )
    {
        VkImageViewCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.image = image;
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = format;
        // NOTE(klek): We are currently not setting the components!
        // info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        // info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        // info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        // info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        // NOTE(klek): Subresources allow the view to view only part 
        //             of the image
        // Which part of image to view
        info.subresourceRange.aspectMask = aspectFlags;
        // Start mipmap level to view from
        info.subresourceRange.baseMipLevel = 0;
        // Number of mipmap levels to view
        info.subresourceRange.levelCount = 1;
        // Start array level to view from
        info.subresourceRange.baseArrayLayer = 0;
        // Number of array levels to view
        info.subresourceRange.layerCount = 1;

        return info;
    }

    VkImageSubresourceRange imageSubresourceRange( const VkImageAspectFlags& aspectMask )
    {
        VkImageSubresourceRange subImage { };
        subImage.aspectMask = aspectMask;
        subImage.baseMipLevel = 0;
        subImage.levelCount = VK_REMAINING_MIP_LEVELS;
        subImage.baseArrayLayer = 0;
        subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

        return subImage;
    }

    void transitionImage( const VkCommandBuffer& cmdBuffer,
                          const VkImage& image,
                          const VkImageLayout currentLayout,
                          const VkImageLayout newLayout )
    {
        VkImageMemoryBarrier barrier { };
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = nullptr;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.oldLayout = currentLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;

        VkImageAspectFlags aspectFlags { };
        aspectFlags = ( ( VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL == newLayout ) ?
                        VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT    );

        barrier.subresourceRange = imageSubresourceRange( aspectFlags );

        vkCmdPipelineBarrier( cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                              0, 0, nullptr, 0, nullptr, 1, &barrier );
    }

    //****************************************************************
    // Pipeline related
    // Helper functions to set create state info structures

    // Sets shader stage create info structure
    VkPipelineShaderStageCreateInfo
    setShaderStageInfo( VkShaderStageFlagBits stage,
                        VkShaderModule module,
                        const char* name )
    {
        VkPipelineShaderStageCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.stage = stage;
        info.module = module;
        info.pName = name;
        info.pSpecializationInfo = nullptr;

        return info;
    }

    // Sets vertex input state create info structure
    VkPipelineVertexInputStateCreateInfo
    setVertexInputInfo( const utils::vector<VkVertexInputBindingDescription>& bindingDesc,
                        const utils::vector<VkVertexInputAttributeDescription>& attributeDesc )
    {
        VkPipelineVertexInputStateCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        // info.vertexBindingDescriptionCount = 0;
        // info.vertexAttributeDescriptionCount = 0;
        info.vertexBindingDescriptionCount = static_cast<uint32_t>( bindingDesc.size() );
        info.pVertexBindingDescriptions = bindingDesc.data();
        info.vertexAttributeDescriptionCount = static_cast<uint32_t>( attributeDesc.size() );
        info.pVertexAttributeDescriptions = attributeDesc.data();
        return info;
    }

    // Sets tesselation state create info structure
    VkPipelineTessellationStateCreateInfo
    setTesselationInfo( void )
    {
        VkPipelineTessellationStateCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.patchControlPoints = 0;
        return info;
    }

    // Sets color blend state create info structure
    VkPipelineColorBlendStateCreateInfo
    setColorBlendInfo( const utils::vector<VkPipelineColorBlendAttachmentState>& attachment )
    {
        VkPipelineColorBlendStateCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.logicOpEnable = VK_FALSE;
        info.logicOp = VK_LOGIC_OP_COPY;
        info.attachmentCount = static_cast<uint32_t>( attachment.size() );
        info.pAttachments = attachment.data();
        info.blendConstants[ 0 ] = 0.0f;
        info.blendConstants[ 1 ] = 0.0f;
        info.blendConstants[ 2 ] = 0.0f;
        info.blendConstants[ 3 ] = 0.0f;
        return info;
    }

    // Sets dynamic state create info structure
    VkPipelineDynamicStateCreateInfo
    setDynamicStateInfo( const utils::vector<VkDynamicState>& dynamicStates )
    {
        VkPipelineDynamicStateCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.dynamicStateCount = static_cast<uint32_t>( dynamicStates.size() );
        info.pDynamicStates = dynamicStates.data();
        return info;
    }

    //****************************************************************
    // Pipeline related
    // Helper functions for default create state info structures
    VkPipelineLayoutCreateInfo
    defaultPipelineLayoutInfo( void )
    {
        VkPipelineLayoutCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.pNext = nullptr;
        info.setLayoutCount = 0;
        info.pSetLayouts = nullptr;
        info.pushConstantRangeCount = 0;
        info.pPushConstantRanges = nullptr;

        return info;
    }

    VkPipelineInputAssemblyStateCreateInfo
    defaultInputAssemblyInfo( void )
    {
        VkPipelineInputAssemblyStateCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        info.primitiveRestartEnable = VK_FALSE;
        return info;
    }

    VkPipelineViewportStateCreateInfo
    defaultViewportInfo( void )
    {
        VkPipelineViewportStateCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.viewportCount = 1;
        // NOTE(klek): Using dynamic states for viewports
        info.pViewports = nullptr;
        info.scissorCount = 1;
        // NOTE(klek): Using dynamic states for scissors
        info.pScissors = nullptr;
        return info;
    }

    VkPipelineRasterizationStateCreateInfo
    defaultRasterizerInfo( void )
    {
        VkPipelineRasterizationStateCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.depthClampEnable = VK_FALSE;
        info.rasterizerDiscardEnable = VK_FALSE;
        info.polygonMode = VK_POLYGON_MODE_FILL;
        info.cullMode = VK_CULL_MODE_NONE;
        info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        info.depthBiasEnable = VK_FALSE;
        info.depthBiasConstantFactor = 0.0f;
        info.depthBiasClamp = 0.0f;
        info.depthBiasSlopeFactor = 0.0f;
        info.lineWidth = 1.0f;
        return info;
    }

    VkPipelineMultisampleStateCreateInfo
    defaultMultiSampleInfo( void )
    {
        VkPipelineMultisampleStateCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        info.sampleShadingEnable = VK_FALSE;
        info.minSampleShading = 1.0f;
        info.pSampleMask = nullptr;
        info.alphaToCoverageEnable = VK_FALSE;
        info.alphaToOneEnable = VK_FALSE;
        return info;
    }

    VkPipelineColorBlendAttachmentState
    defaultColorBlendAttachmentInfo( void )
    {
        VkPipelineColorBlendAttachmentState info { };
        info.blendEnable = VK_FALSE;
        info.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        info.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        info.colorBlendOp = VK_BLEND_OP_ADD;
        info.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        info.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        info.alphaBlendOp = VK_BLEND_OP_ADD;
        info.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
                              VK_COLOR_COMPONENT_G_BIT | 
                              VK_COLOR_COMPONENT_B_BIT | 
                              VK_COLOR_COMPONENT_A_BIT;
        return info;
    }

    VkPipelineDepthStencilStateCreateInfo
    defaultDepthStencilInfo( void )
    {
        VkPipelineDepthStencilStateCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.depthTestEnable = VK_TRUE;
        info.depthWriteEnable = VK_TRUE;
        info.depthCompareOp = VK_COMPARE_OP_LESS;
        info.depthBoundsTestEnable = VK_FALSE;
        info.stencilTestEnable = VK_FALSE;
        info.front = { };
        info.back = { };
        info.minDepthBounds = 0.0f;
        info.maxDepthBounds = 1.0f;
        return info;
    }

} // namespace muggy::graphics::vulkan::vulkan_utils
